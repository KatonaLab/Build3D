#ifndef _core_high_platform_IcsAdapter_h_
#define _core_high_platform_IcsAdapter_h_

#include <core/multidim_image_platform/MultiDimImage.hpp>
// for MultiDimImage PORT_TYPE_TRAITS reg
#include <core/high_platform/PythonComputeModule.h>

#include <type_traits>
#include <typeindex>

#include <libics.h>
#include <libics_intern.h>
#include <cstdio>

namespace md = core::multidim_image_platform;

namespace core {
namespace io_utils {

class IcsAdapter {
public:
    bool open(const std::string& filename);
    template <typename T> md::MultiDimImage<T> read(bool xyztcReorder = false);
    // TODO: write test for this function
    template <typename T> md::MultiDimImage<T> readConvert(bool xyztcReorder = false);
    template <typename T> md::MultiDimImage<T> readScaledConvert(bool xyztcReorder = false);
    template <typename T> void write(const std::string& filename, const md::MultiDimImage<T>& data);
    std::type_index dataType() const;
    bool valid() const;
    void close();
    virtual ~IcsAdapter();
protected:
    void errorCheck(Ics_Error error, const std::string message);
    template <typename T> md::MultiDimImage<T> readOriginalChannelOrder();
    template <typename T> md::MultiDimImage<T> readModifiedChannelOrder();
protected:
    std::string m_filename;
    ICS *m_ip = nullptr;
    Ics_DataType m_dt = Ics_unknown;
    std::vector<std::size_t> m_reorder;
    std::vector<std::size_t> m_dims = std::vector<std::size_t>(ICS_MAXDIM, 0);
};

template <typename T>
md::MultiDimImage<T> IcsAdapter::readOriginalChannelOrder()
{
    md::MultiDimImage<T> im(m_dims);
    std::size_t size = 0;
    if (m_dims.empty()) {
        return im;
    } else if (m_dims.size() == 1) {
        size = m_dims[0];
    } else {
        size = m_dims[0] * m_dims[1];
    }

    auto& planes = im.unsafeData();
    for (auto& plane : planes) {
        IcsGetDataBlock(m_ip, plane.data(), size * sizeof(T));
    }
    // NOTE: a dirty hack for rewinding the ics file pointer since there is
    // no support for that in libics and I don't want IcsClose/IcsOpen all the time
    std::rewind(((Ics_BlockRead*)m_ip->blockRead)->dataFilePtr);

    return im;
}

template <typename T>
md::MultiDimImage<T> IcsAdapter::readModifiedChannelOrder()
{
    auto im = readOriginalChannelOrder<T>();
    im.reorderDims(m_reorder);
    return im;
}

template <typename T>
md::MultiDimImage<T> IcsAdapter::read(bool xyztcReorder)
{
    if (!valid()) {
        throw std::runtime_error("can not read '" + m_filename + "', check the .ics file and also the corresponding .ids file");
    }

    if (std::type_index(typeid(T)) != dataType()) {
        throw std::runtime_error("image object and ics file data type mismatch in '" + m_filename + "'");
    }

    if (xyztcReorder) {
        return readModifiedChannelOrder<T>();
    } else {
        return readOriginalChannelOrder<T>();
    }
}

template <typename T>
md::MultiDimImage<T> IcsAdapter::readConvert(bool xyztcReorder)
{
    if (!valid()) {
        throw std::runtime_error("can not read '" + m_filename + "', check the .ics file and also the corresponding .ids file");
    }

    if (std::type_index(typeid(T)) != dataType()) {
        md::MultiDimImage<T> im;
        switch (m_dt) {
            case Ics_uint8:  im.convertCopyFrom(read<uint8_t>(xyztcReorder)); break;
            case Ics_sint8:  im.convertCopyFrom(read<int8_t>(xyztcReorder)); break;
            case Ics_uint16: im.convertCopyFrom(read<uint16_t>(xyztcReorder)); break;
            case Ics_sint16: im.convertCopyFrom(read<int16_t>(xyztcReorder)); break;
            case Ics_uint32: im.convertCopyFrom(read<uint32_t>(xyztcReorder)); break;
            case Ics_sint32: im.convertCopyFrom(read<int32_t>(xyztcReorder)); break;
            case Ics_real32: im.convertCopyFrom(read<float>(xyztcReorder)); break;
            case Ics_real64: im.convertCopyFrom(read<double>(xyztcReorder)); break;
            case Ics_complex32:
            case Ics_complex64:
            case Ics_unknown:
            default: throw std::runtime_error("unsupported data type for scaled convert read in '" + m_filename + "'");
        }
        return im;
    }

    return read<T>(xyztcReorder);
}

template <typename T>
md::MultiDimImage<T> IcsAdapter::readScaledConvert(bool xyztcReorder)
{
    if (!valid()) {
        throw std::runtime_error("can not read '" + m_filename + "', check the .ics file and also the corresponding .ids file");
    }
    
    if (std::type_index(typeid(T)) != dataType()) {
        md::MultiDimImage<T> im;
        switch (m_dt) {
            case Ics_uint8:  im.scaledCopyFrom(read<uint8_t>(xyztcReorder)); break;
            case Ics_sint8:  im.scaledCopyFrom(read<int8_t>(xyztcReorder)); break;
            case Ics_uint16: im.scaledCopyFrom(read<uint16_t>(xyztcReorder)); break;
            case Ics_sint16: im.scaledCopyFrom(read<int16_t>(xyztcReorder)); break;
            case Ics_uint32: im.scaledCopyFrom(read<uint32_t>(xyztcReorder)); break;
            case Ics_sint32: im.scaledCopyFrom(read<int32_t>(xyztcReorder)); break;
            case Ics_real32: im.scaledCopyFrom(read<float>(xyztcReorder)); break;
            case Ics_real64: im.scaledCopyFrom(read<double>(xyztcReorder)); break;
            case Ics_complex32:
            case Ics_complex64:
            case Ics_unknown:
            default: throw std::runtime_error("unsupported data type for scaled convert read in '" + m_filename + "'");
        }
        return im;
    }

    return read<T>(xyztcReorder);
}

template <typename T>
void IcsAdapter::write(const std::string& /*filename*/, const md::MultiDimImage<T>& /*data*/)
{
    // TODO:
}

}
}

#endif
