#include "VolumeTexture.h"
#include "VolumeData.h"

#include <QTextureWrapMode>

using namespace std;
using namespace core::multidim_image_platform;
using namespace Qt3DCore;
using namespace Qt3DRender;

VolumeTexture::VolumeTexture(Qt3DCore::QNode *parent)
    : Qt3DRender::QAbstractTexture(QAbstractTexture::Target3D, parent)
{}

VolumeTexture::~VolumeTexture()
{
    if (m_textureImage) {
        removeTextureImage(m_textureImage.get());
    }
}

void VolumeTexture::init(MultiDimImage<float>& source)
{
    if (m_textureImage) {
        removeTextureImage(m_textureImage.get());
    }

    m_data = unique_ptr<VolumeData>(new VolumeData(source));
    m_textureImage = unique_ptr<VolumeTextureImage>(new VolumeTextureImage(*m_data));

    setWrapMode(Qt3DRender::QTextureWrapMode(QTextureWrapMode::ClampToBorder));
    setSmooth(false);
    addTextureImage(m_textureImage.get());
}

void VolumeTexture::setSmooth(bool val)
{
    if (val) {
        setMinificationFilter(Qt3DRender::QAbstractTexture::Filter::Linear);
        setMagnificationFilter(Qt3DRender::QAbstractTexture::Filter::Linear);
    } else {
        setMinificationFilter(Qt3DRender::QAbstractTexture::Filter::Nearest);
        setMagnificationFilter(Qt3DRender::QAbstractTexture::Filter::Nearest);
    }
    m_smooth = val;
}

VolumeTextureImage::VolumeTextureImage(const VolumeData& data, QNode *parent)
    : QAbstractTextureImage(parent)
{
    m_generator = VolumeTextureImageDataGeneratorPtr::create(data);
}

QTextureImageDataGeneratorPtr VolumeTextureImage::dataGenerator() const
{
    return m_generator;
}

VolumeTextureImageDataGenerator::VolumeTextureImageDataGenerator(const VolumeData& data)
    : m_data(data)
{}

QTextureImageDataPtr VolumeTextureImageDataGenerator::operator()()
{
    QTextureImageDataPtr texImage = QTextureImageDataPtr::create();

    texImage->setWidth(m_data.width());
    texImage->setHeight(m_data.height());
    texImage->setDepth(m_data.depth());
    texImage->setFaces(1);
    texImage->setLayers(1);
    texImage->setMipLevels(1);
    texImage->setFormat(QOpenGLTexture::TextureFormat::R32F);
    texImage->setPixelFormat(QOpenGLTexture::PixelFormat::Red);
    texImage->setPixelType(QOpenGLTexture::PixelType::Float32);
    texImage->setTarget(QOpenGLTexture::Target::Target3D);

    texImage->setData(m_data.toQByteArray(), sizeof(float));

    return texImage;
}

bool VolumeTextureImageDataGenerator::operator==(const QTextureImageDataGenerator &other) const
{
    const VolumeTextureImageDataGenerator* otherFunctor = functor_cast<VolumeTextureImageDataGenerator>(&other);
    return (otherFunctor != Q_NULLPTR && &(otherFunctor->m_data) == &m_data);
}
