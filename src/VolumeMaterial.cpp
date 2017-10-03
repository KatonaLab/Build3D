#include "VolumeMaterial.h"

using namespace Qt3DRender;

QTextureImageDataGeneratorPtr CustomDataTextureImage::dataGenerator() const
{
    return generator;
}

void CustomDataTextureImage::setData(const uchar *data, int width, int height)
{
    generator->setData(data, width, height);
}

CustomDataTextureImage::CustomDataTextureImage(Qt3DCore::QNode *parent)
{
    generator = CustomDataTextureImageGeneratorPtr::create();
}

// -----------------------------------------------------------------------------

void CustomDataTextureImageGenerator::setData(const uchar *data, int width, int height)
{
    buf = data;
    this->width = width;
    this->height = height;
}

QTextureImageDataPtr CustomDataTextureImageGenerator::operator()()
{
    QTextureImageDataPtr texImage = QTextureImageDataPtr::create();

    texImage->setWidth(width);
    texImage->setHeight(height);
    texImage->setDepth(1);
    texImage->setFaces(1);
    texImage->setLayers(15);
    texImage->setMipLevels(1);
    texImage->setFormat(QOpenGLTexture::TextureFormat::RGBA8U);
    texImage->setPixelFormat(QOpenGLTexture::PixelFormat::Luminance);
    texImage->setPixelType(QOpenGLTexture::PixelType::Float32);
    texImage->setTarget(QOpenGLTexture::Target::Target2D);

//    texImage->setImage(<#const QImage &#>)

//    const float *fdata = (const float*)buf;
//    for (int i = 0; i < 512; ++i) {
//        qDebug() << fdata[i];
//    }

//    const QByteArray bytes = QByteArray(reinterpret_cast<const char*>(buf), width * height);
    const QByteArray bytes = QByteArray(reinterpret_cast<const char*>(buf), width * height * 4);

//    uchar *b = new uchar[width * height * 4];
//    for (int i = 0; i < height; ++i) {
//        for (int j = 0; j < width; ++j) {
//            b[(width * i + j) * 4 + 0] = j;
//            b[(width * i + j) * 4 + 1] = 0;
//            b[(width * i + j) * 4 + 2] = 0;
//            b[(width * i + j) * 4 + 3] = 255;
//        }
//    }
//    const QByteArray bytes = QByteArray((char*)b, width * height * 4);
    texImage->setData(bytes, width * height * 4);

    return texImage;
}

bool CustomDataTextureImageGenerator::operator ==(const QTextureImageDataGenerator &other) const
{
    return true;
}
