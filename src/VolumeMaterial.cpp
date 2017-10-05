#include "VolumeMaterial.h"

using namespace Qt3DRender;

DynamicVolumetricTexture::DynamicVolumetricTexture(Qt3DCore::QNode *parent)
{}

DynamicVolumetricTexture::~DynamicVolumetricTexture()
{}

void DynamicVolumetricTexture::setData(const uchar *data, int width, int height)
{
//    setDataFunctor()
//    texImage.setData(data, width, height);
//    cout << "removing texture" << endl;
//    this->texImage
//    this->addTextureImage(&texImage);
}

// -----------------------------------------------------------------------------

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
    texImage->setLayers(1);
    texImage->setMipLevels(1);
    texImage->setFormat(QOpenGLTexture::TextureFormat::RGBA8_UNorm);

    texImage->setPixelFormat(QOpenGLTexture::PixelFormat::Red);
    texImage->setPixelType(QOpenGLTexture::PixelType::UInt8);

//    texImage->setPixelFormat(QOpenGLTexture::PixelFormat::Red);
//    texImage->setPixelType(QOpenGLTexture::PixelType::Float32);

    texImage->setTarget(QOpenGLTexture::Target::Target2D);

    const QByteArray bytes = QByteArray(reinterpret_cast<const char*>(buf), width * height * 4);
    texImage->setData(bytes, 1);
    return texImage;

//    const float *fdata = (const float*)buf;
//    for (int i = 0; i < 512; ++i) {
//        qDebug() << fdata[i];
//    }

//    const QByteArray bytes = QByteArray(reinterpret_cast<const char*>(buf), width * height);
//    texImage->setData(bytes, 1);
//    return texImage;

//    const QByteArray bytes = QByteArray(reinterpret_cast<const char*>(buf), width * height);

//    uchar *b = new uchar[width * height];
//    for (int i = 0; i < height; ++i) {
//        for (int j = 0; j < width; ++j) {
//            b[width * i + j] = j;
////            b[(width * i + j) * 4 + 0] = j;
////            b[(width * i + j) * 4 + 1] = i;
////            b[(width * i + j) * 4 + 2] = 0;
////            b[(width * i + j) * 4 + 3] = 255;
//        }
//    }
//    const QByteArray bytes = QByteArray(reinterpret_cast<const char*>(b), width * height * 1);
//    texImage->setData(bytes, 1);
//
//    return texImage;
}

bool CustomDataTextureImageGenerator::operator ==(const QTextureImageDataGenerator &other) const
{
    return true;
}
