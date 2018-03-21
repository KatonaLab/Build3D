#ifndef _app_VolumeTexture_h_
#define _app_VolumeTexture_h_

#include <QtCore>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageData>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <QVector3D>
#include <core/multidim_image_platform/MultiDimImage.hpp>
#include "VolumeData.h"
#include <memory>

class VolumeTextureImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator {
public:
    QT3D_FUNCTOR(VolumeTextureImageDataGenerator)
    VolumeTextureImageDataGenerator(const VolumeData& data);
    virtual ~VolumeTextureImageDataGenerator() = default;
    Qt3DRender::QTextureImageDataPtr operator()() override;
    bool operator==(const QTextureImageDataGenerator &other) const override;
private:
    const VolumeData& m_data;
};

typedef QSharedPointer<VolumeTextureImageDataGenerator> VolumeTextureImageDataGeneratorPtr;

class VolumeTextureImage : public Qt3DRender::QAbstractTextureImage {
    Q_OBJECT
public:
    explicit VolumeTextureImage(const VolumeData& data, Qt3DCore::QNode *parent = nullptr);
    virtual ~VolumeTextureImage() = default;
protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
    VolumeTextureImageDataGeneratorPtr m_generator;
};

typedef QSharedPointer<VolumeTextureImageDataGenerator> VolumeTextureImageDataGeneratorPtr;

class VolumeTexture : public Qt3DRender::QAbstractTexture {
    Q_OBJECT
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth)
    Q_PROPERTY(QVector3D size READ size)
public:
    explicit VolumeTexture(Qt3DCore::QNode* parent = nullptr);
    void init(core::multidim_image_platform::MultiDimImage<float>& source, std::size_t channel);
    QVector3D size() const { return QVector3D(m_data->width(), m_data->height(), m_data->depth()); }
    virtual ~VolumeTexture() = default;
    bool smooth() const { return m_smooth; }
    void setSmooth(bool val);
protected:
    std::unique_ptr<VolumeData> m_data;
    std::unique_ptr<VolumeTextureImage> m_textureImage;
    bool m_smooth = false;
};

#endif
