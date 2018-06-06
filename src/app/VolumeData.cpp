#include "VolumeData.h"

using namespace std;
using namespace core::multidim_image_platform;

VolumeData::VolumeData(MultiDimImage<float> &source)
    : m_source(source)
{
    if (source.dims() != 3) {
        throw std::runtime_error("VolumeData accepts 3D MultiDimImage only");
    }
}

QByteArray VolumeData::toQByteArray() const
{
    size_t xySize = width() * height();
    size_t xyByteSize = xySize * sizeof(float);
    size_t xyzSize = xySize * depth();
    size_t xyzByteSize = xyzSize * sizeof(float);

    size_t zSize = depth();

    QByteArray data;
    data.resize(xyzByteSize);

    // char* data = new char[xyzByteSize];
    // if (data == nullptr) {
    //     throw std::runtime_error("can not allocate enough memory for the data");
    // }

    auto& planes = m_source.unsafeData();
    for (size_t i = 0; i < zSize; ++i) {
        memcpy(data.data() + i * xyByteSize, planes[i].data(), xyByteSize);
    }

    // return QByteArray().fromRawData(data, xyzByteSize);
    return data;
}