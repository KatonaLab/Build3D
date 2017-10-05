#include "volumetric.h"
#include <libics.h>

using namespace std;
using namespace Qt3DCore;
using namespace Qt3DRender;

vector<VolumetricDataPtr> VolumetricData::loadICS(string filename)
{
    vector<VolumetricDataPtr> vdList;

    // TODO: use IcsGetOrder and prepare for shuffled dim order
    ICS* ip;
    IcsOpen(&ip, filename.c_str(), "r");

    Ics_DataType dt;
    int ndims;
    size_t dims[ICS_MAXDIM];
    IcsGetLayout(ip, &dt, &ndims, dims);
    size_t imageElementSize = IcsGetImelSize(ip);

    size_t w, h, d, c, n;
    c = ndims >= 4 ? dims[3] : 1;
    d = ndims >= 3 ? dims[2] : 1;
    h = ndims >= 2 ? dims[1] : 1;
    w = ndims >= 1 ? dims[0] : 1;
    n = w * h * d;

    ptrdiff_t strides[ndims];
    for (int j = 0; j < ndims; ++j) {
        strides[j] = 1;
        if (j == 3) {
            strides[j] = c;
        }
    }

    for (int k = 0; k < c; ++k) {
        VolumetricDataPtr vd = VolumetricDataPtr::create();
        vd->m_data = new char[w * h];
        vd->m_dims[0] = w;
        vd->m_dims[1] = h;
        vd->m_dims[2] = 1;
        IcsGetPreviewData(ip, vd->m_data, w * h, k * d);
        vdList.push_back(vd);
    }

    IcsClose(ip);
    return vdList;
}

VolumetricTexture::VolumetricTexture(Qt3DCore::QNode *parent)
: Qt3DRender::QAbstractTexture(parent)
{}

void VolumetricTexture::setDataSource(const VolumetricDataPtr data)
{
    if (m_textureImage) {
        removeTextureImage(m_textureImage);
        delete m_textureImage;
        m_textureImage = nullptr;
    }

    m_textureImage = new VolumetricTextureImage(data);
    addTextureImage(m_textureImage);
}

VolumetricTexture::~VolumetricTexture()
{
    delete m_textureImage;
}

//------------------------------------------------------------------------------

VolumetricTextureImage::VolumetricTextureImage(const VolumetricDataPtr data, QNode *parent)
: QAbstractTextureImage(parent)
{
    m_generator = ImageDataGeneratorPtr::create(data);
}

QTextureImageDataGeneratorPtr VolumetricTextureImage::dataGenerator() const
{
    return m_generator;
}

//------------------------------------------------------------------------------

ImageDataGenerator::ImageDataGenerator(VolumetricDataPtr data) : m_data(data)
{}

QTextureImageDataPtr ImageDataGenerator::operator()()
{
    QTextureImageDataPtr texImage = QTextureImageDataPtr::create();

    texImage->setWidth(m_data->width());
    texImage->setHeight(m_data->height());
    texImage->setDepth(1);
    texImage->setFaces(1);
    texImage->setLayers(1);
    texImage->setMipLevels(1);
    texImage->setFormat(QOpenGLTexture::TextureFormat::RGBA8_UNorm);

    texImage->setPixelFormat(QOpenGLTexture::PixelFormat::Red);
    texImage->setPixelType(QOpenGLTexture::PixelType::UInt8);

    texImage->setTarget(QOpenGLTexture::Target::Target2D);

    const QByteArray bytes = QByteArray(m_data->data(), m_data->width() * m_data->height());
    texImage->setData(bytes, 1);

    return texImage;
}

bool ImageDataGenerator::operator ==(const QTextureImageDataGenerator &other) const
{
    return true;
}
