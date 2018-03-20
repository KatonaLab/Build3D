#include "VolumeData.h"

using namespace std;
using namespace core::multidim_image_platform;

VolumeData::VolumeData(MultiDimImage<float> &source, std::size_t channel)
    : m_source(source), m_channel(channel)
{
    if (source.dims() != 4) {
        throw std::runtime_error("VolumeData accepts 4D MultiDimImage only");
    }
}

QByteArray VolumeData::toQByteArray() const
{
    size_t xySize = width() * height();
    size_t xyByteSize = xySize * sizeof(float);
    size_t xyzSize = xySize * depth();
    size_t xyzByteSize = xyzSize * sizeof(float);

    size_t zSize = depth();
    size_t offset = xyzSize * m_channel;

    char* data = new char[xyzByteSize];
    auto& planes = m_source.unsafeData();
    for (size_t i = 0; i < zSize; ++i) {
        memcpy(data + i * xyByteSize, planes[i + offset].data(), xyByteSize);
    }

    return QByteArray().fromRawData(data, xyzByteSize);
}