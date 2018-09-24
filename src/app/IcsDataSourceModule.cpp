#include "IcsDataSourceModule.h"

#include <core/io_utils/IcsAdapter.h>
#include <QDebug>

using namespace std;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
using namespace core::io_utils;

bool IcsDataSourceModule::modifiedParameters()
{
    // TODO: caching mechanism should be done on a higher level -> ComputeModule
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

void IcsDataSourceModule::execute(ModuleContext&)
{
    // TODO: raise exception on error!
    if (modifiedParameters()) {

        IcsAdapter ics;
        string filePath = m_lastPathValue.path;

        ics.open(filePath);

        MultiDimImage<float> image;
        Meta meta = ics.getMeta();

        if (m_inputs.input<1>()->value()) {
            image = ics.readScaledConvert<float>(true);
            meta.add("normalized", "true");
        } else {
            image = ics.readConvert<float>(true);
            meta.add("normalized", "false");
        }

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
        }

        int n = min((size_t)4, m_cache.size());
        for (int i = 0; i < n; ++i) {
            if (m_cache[i]) {
                m_cache[i]->meta = meta;
                m_cache[i]->meta.add("channel", to_string(i));
                string wlKey = "IcsGetSensorEmissionWavelength:" + to_string(i);
                if (meta.has(wlKey)) {
                    m_cache[i]->meta.add("wavelength", meta.get(wlKey));
                }
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

// -------------------------------

bool TwoChannelIcsModule::modifiedParameters()
{
    // TODO: caching mechanism should be done on a higher level -> ComputeModule
    bool r = false;
    if (m_lastPathValue.path != m_inputs.input<0>()->value().path) {
        m_lastPathValue = m_inputs.input<0>()->value();
        r = true;
    }

    return r;
}

TwoChannelIcsModule::TwoChannelIcsModule(ComputePlatform& parent)
    : ComputeModule(parent, m_inputs, m_outputs, "IcsDataSource"),
    m_inputs(*this),
    m_outputs(*this)
{
    m_inputs.input<0>()->setName("file");

    m_inputs.input<1>()->setName("channel 1");
    PropertyMap& ch1 = m_inputs.input<1>()->properties();
    ch1.setBool("parameter", true);
    ch1.setInt("min", 0);
    ch1.setInt("default", 0);
    // ch1.setInt("max", 8);

    m_inputs.input<2>()->setName("channel 2");
    PropertyMap& ch2 = m_inputs.input<2>()->properties();
    ch2.setBool("parameter", true);
    ch2.setInt("min", 0);
    ch2.setInt("default", 1);
    // ch2.setInt("max", 8);

    m_outputs.output<0>()->setName("channel 1");
    m_outputs.output<1>()->setName("channel 2");
}

void TwoChannelIcsModule::execute(ModuleContext&)
{
    // TODO: raise exception on error!
    if (modifiedParameters()) {

        IcsAdapter ics;
        string filePath = m_lastPathValue.path;

        ics.open(filePath);

        MultiDimImage<float> image;
        Meta meta = ics.getMeta();

        image = ics.readConvert<float>(true);
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
        }

        int n = min((size_t)8, m_cache.size());
        for (int i = 0; i < n; ++i) {
            if (m_cache[i]) {
                m_cache[i]->meta = meta;
                m_cache[i]->meta.add("channel", to_string(i));
                string wlKey = "IcsGetSensorEmissionWavelength:" + to_string(i);
                if (meta.has(wlKey)) {
                    m_cache[i]->meta.add("wavelength", meta.get(wlKey));
                }
            }
        }
    }

    int chs[2];
    chs[0] = (int)m_inputs.input<1>()->value();
    chs[1] = (int)m_inputs.input<2>()->value();

    #define OUT(i) \
        if (0 <= chs[(i)] && chs[(i)] < (int)m_cache.size()) { \
            m_outputs.output<(i)>()->forwardFromSharedPtr(m_cache[chs[(i)]]); \
        } else { \
            shared_ptr<MultiDimImage<float>> empty; \
            m_outputs.output<(i)>()->forwardFromSharedPtr(empty); \
            qWarning() << "channel" << (chs[(i)] + 1) << "for" \
                << QString::fromStdString(m_lastPathValue.path) << "is empty"; \
        }

    OUT(0)
    OUT(1)

    #undef OUT
}
