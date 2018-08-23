#include "IcsDataSourceModule.h"

#include <core/io_utils/IcsAdapter.h>

using namespace std;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
using namespace core::io_utils;

bool IcsDataSourceModule::modifiedParameters()
{
    bool r = false;
    if (m_lastPathValue.path != m_inputs.input<0>()->value().path) {
        m_lastPathValue = m_inputs.input<0>()->value();
        r = true;
    }

    if (m_lastNormalizeValue != m_inputs.input<1>()->value()) {
        m_lastNormalizeValue = m_inputs.input<1>()->value();
        r = true;
    }

    return r;
}

IcsDataSourceModule::IcsDataSourceModule(ComputePlatform& parent)
    : ComputeModule(parent, m_inputs, m_outputs, "IcsDataSource"),
    m_inputs(*this),
    m_outputs(*this)
{
    m_inputs.input<0>()->setName("file");
    m_inputs.input<0>()->properties().setBool("parameter", true);

    m_inputs.input<1>()->setName("normalize values to [0, 1]");
    m_inputs.input<1>()->properties().setBool("parameter", true);

    m_outputs.output<0>()->setName("channel 0");
    m_outputs.output<1>()->setName("channel 1");
    m_outputs.output<2>()->setName("channel 2");
    m_outputs.output<3>()->setName("channel 3");
}

void IcsDataSourceModule::execute()
{
    // TODO: raise exception on error!
    if (modifiedParameters()) {

        IcsAdapter ics;

        QUrl url;
        url.setUrl(QString::fromStdString(m_lastPathValue.path));
        string filePath = url.toLocalFile().toStdString();

        ics.open(filePath);

        MultiDimImage<float> image;
        Meta meta;

        if (m_inputs.input<1>()->value()) {
            image = ics.readScaledConvert<float>(true);
            meta.add("normalized", "true");
        } else {
            image = ics.readConvert<float>(true);
            meta.add("normalized", "false");
        }

        string typeName;
        switch (ics.icsDataType()) {
            case Ics_uint8: typeName = "uint8"; break;
            case Ics_sint8: typeName = "int8"; break;
            case Ics_uint16: typeName = "uint16"; break;
            case Ics_sint16: typeName = "int16"; break;
            case Ics_uint32: typeName = "uint32"; break;
            case Ics_sint32: typeName = "int32"; break;
            case Ics_real32: typeName = "float"; break;
            case Ics_real64: typeName = "double"; break;
            case Ics_complex32:
            case Ics_complex64:
            case Ics_unknown:
            default: typeName = "unknown";
        }
        meta.add("type", typeName);
        meta.add("path", filePath);

        m_cache.clear();

        switch (image.dims()) {
            case 3: // xyz
                m_cache.push_back(make_shared<MultiDimImage<float>>(image));
                break;
            case 5: // xyztc
                // removing dimension 't'
                image.removeDims({3});
                // no break - let it flow
            case 4: { // xyzc or xyzt
                auto volumes = image.splitDim(3);
                int n = min((size_t)4, volumes.size());
                for (int i = 0; i < n; ++i) {
                    m_cache.push_back(make_shared<MultiDimImage<float>>(move(volumes[i])));
                }
                break;
            }
        // TODO: support XY images too
        default:
        // TODO: proper error message to GUI
        throw std::runtime_error("ICS files with dimensions XYZ or XYZC are supported only");
        };

        int n = min((size_t)4, m_cache.size());
        for (int i = 0; i < n; ++i) {
            if (m_cache[i]) {
                m_cache[i]->meta = meta;
                m_cache[i]->meta.add("channel", to_string(i));
            }
        }
    }

    #define OUT(i) \
        if (m_cache.size() > (i)) { \
            m_outputs.output<(i)>()->forwardFromSharedPtr(m_cache[(i)]); \
        } else { \
            shared_ptr<MultiDimImage<float>> empty; \
            m_outputs.output<(i)>()->forwardFromSharedPtr(empty); \
        }

    OUT(0)
    OUT(1)
    OUT(2)
    OUT(3)

    #undef OUT
}
