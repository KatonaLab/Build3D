#include "VolumeData.h"

using namespace std;
using namespace core::multidim_image_platform;

VolumeData::VolumeData(std::shared_ptr<MultiDimImage<float>> source)
    : m_source(source)
{
    if (!source) {
        throw std::runtime_error("VolumeData accepts valid MultiDimImage only");
    }
    if (m_source->dims() != 3) {
        throw std::runtime_error("VolumeData accepts 3D MultiDimImage only");
    }
}

bool VolumeData::valid() const
{
    return (bool)m_source;
}

QByteArray VolumeData::toQByteArray() const
{
    size_t xySize = width() * height();
    size_t xyByteSize = xySize * sizeof(float);
    size_t xyzSize = xySize * depth();
    size_t xyzByteSize = xyzSize * sizeof(float);

    size_t zSize = depth();

    // let QByteArray allocate and release its data
    QByteArray data;
    data.resize(xyzByteSize);

    if (!valid()) {
        throw std::runtime_error("invalid VolumeData");
    }

    auto& planes = m_source->unsafeData();
    for (size_t i = 0; i < zSize; ++i) {
        memcpy(data.data() + i * xyByteSize, planes[i].data(), xyByteSize);
    }

    return data;
}

size_t VolumeData::width() const
{
    if (!valid()) {
        throw std::runtime_error("invalid VolumeData");
    }
    return m_source->dim(0);
}

size_t VolumeData::height() const
{
    if (!valid()) {
        throw std::runtime_error("invalid VolumeData");
    }
    return m_source->dim(1);
}

size_t VolumeData::depth() const
{
    if (!valid()) {
        throw std::runtime_error("invalid VolumeData");
    }
    return m_source->dim(2);

}
