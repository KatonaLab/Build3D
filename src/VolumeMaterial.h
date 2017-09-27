#ifndef _VolumeMaterial_h_
#define _VolumeMaterial_h_

#include <Qt3DExtras/QTextureMaterial>

class VolumeMaterial : public Qt3DExtras::QTextureMaterial {
    Q_OBJECT
public:

};

#include <Qt3DRender/QTextureImageDataGenerator>

class CustomDataTextureImageGenerator : public Qt3DRender::QTextureImageDataGenerator {
public:
    Qt3DRender::QTextureImageDataPtr operator()() override;
    bool operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const override;
};

#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>

class CustomDataTextureImage : public Qt3DRender::QAbstractTextureImage {
    Q_OBJECT
public:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
    void setData(const uchar *data, int width, int height);
};

#endif
