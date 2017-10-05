#ifndef _VolumeMaterial_h_
#define _VolumeMaterial_h_

#include <Qt3DExtras/QTextureMaterial>

class VolumeMaterial : public Qt3DExtras::QTextureMaterial {
    Q_OBJECT
public:

};

#include <Qt3DRender/QAbstractTexture>

class DynamicVolumetricTexture : public Qt3DRender::QAbstractTexture {
    Q_OBJECT
public:
    explicit DynamicVolumetricTexture(Qt3DCore::QNode *parent = nullptr);
    ~DynamicVolumetricTexture();
    void setData(const uchar *data, int width, int height);
};

#include <Qt3DRender/QTextureImageDataGenerator>

class CustomDataTextureImageGenerator : public Qt3DRender::QTextureImageDataGenerator {
    QT3D_FUNCTOR(CustomDataTextureImageGenerator)
public:
    CustomDataTextureImageGenerator() {
        width = 1;
        height = 1;
        buf = &dummy;
    }
    void setData(const uchar *data, int width, int height);
    Qt3DRender::QTextureImageDataPtr operator()() override;
    bool operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const override;
private:
    int width = 0;
    int height = 0;
    const uchar *buf = nullptr;
    const uchar dummy = 0;
};

typedef QSharedPointer<CustomDataTextureImageGenerator> CustomDataTextureImageGeneratorPtr;

#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>

class CustomDataTextureImage : public Qt3DRender::QAbstractTextureImage {
    Q_OBJECT
public:
    explicit CustomDataTextureImage(Qt3DCore::QNode *parent = nullptr);
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
    void setData(const uchar *data, int width, int height);
private:
    CustomDataTextureImageGeneratorPtr generator;
};

#endif
