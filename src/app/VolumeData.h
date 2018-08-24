#ifndef _app_VolumeData_h_
#define _app_VolumeData_h_

#include <QtCore>
#include <core/multidim_image_platform/MultiDimImage.hpp>

// TODO: consider moving toQByteArray functionality to e.g. VolumeTexture::fromMultiDimImage()
// and removing VolumeData

class VolumeData {
public:
    VolumeData(std::shared_ptr<core::multidim_image_platform::MultiDimImage<float>> source);
    size_t width() const;
    size_t height() const;
    size_t depth() const;
    bool valid() const;
    QByteArray toQByteArray() const;
    virtual ~VolumeData() = default;
protected:
    std::shared_ptr<core::multidim_image_platform::MultiDimImage<float>> m_source;
};

#endif
