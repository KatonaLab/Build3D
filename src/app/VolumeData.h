#ifndef _app_VolumeData_h_
#define _app_VolumeData_h_

#include <QtCore>
#include <core/multidim_image_platform/MultiDimImage.hpp>

class VolumeData {
public:
    VolumeData(core::multidim_image_platform::MultiDimImage<float> &source, std::size_t channel);
    size_t width() const { return m_source.dim(0); }
    size_t height() const { return m_source.dim(1); }
    size_t depth() const { return m_source.dim(2); }
    QByteArray toQByteArray() const;
    virtual ~VolumeData() = default;
protected:
    core::multidim_image_platform::MultiDimImage<float> &m_source;
    std::size_t m_channel;
};

#endif
