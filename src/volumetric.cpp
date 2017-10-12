#include "volumetric.h"

#include <iostream>

using namespace std;
using namespace Qt3DCore;
using namespace Qt3DRender;

const map<Ics_DataType, ICSFile::TypeInfoBasePtr> ICSFile::typeMap = {
    {Ics_uint8, ti<uint8_t>()},
    {Ics_sint8, ti<int8_t>()},
    {Ics_uint16, ti<uint16_t>()},
    {Ics_sint16, ti<int16_t>()},
    {Ics_uint32, ti<uint32_t>()},
    {Ics_sint32, ti<int32_t>()},
    {Ics_real32, ti<float>()},
    {Ics_real64, ti<double>()}
};

// TODO: proper error message
#define ICS_EC(call) errorCheck(call, std::string(#call) + " - " + filename)

ICSFile::ICSFile(std::string filename) : filename(filename)
{
    ICS_EC(IcsOpen(&ip, filename.c_str(), "r"));
    ICS_EC(IcsGetLayout(ip, &dt, &ndims, dims));
    if (typeMap.count(dt) == 0) {
        throw ICSError("not supported data type format");
    }
    fillChannelData();
}

void ICSFile::fillChannelData()
{
    size_t size = IcsGetDataSize(ip);
    shared_ptr<char> buffer(new char[size], default_delete<char[]>());
    ICS_EC(IcsGetData(ip, buffer.get(), size));

    size_t n = width() * height() * depth();
    size_t c = channels();
    for (size_t k = 0; k < c; ++k) {
        channelData.emplace_back(new float[n], default_delete<float[]>());
    }

    using namespace std::placeholders;
    auto cast = bind(&TypeInfoBase::cast, typeMap.at(dt), _1);
    size_t bytes = typeMap.at(dt)->bytes();
    for (int k = 0; k < c; ++k) {
        size_t offset = n * k;
        for (size_t i = 0; i < n; ++i) {
            channelData[k].get()[i] = cast(buffer.get() + (i + offset) * bytes) / 1000.;
        }
    }
}

size_t ICSFile::channels() const
{
    // TODO: handle order in ics
    return dims[3];
}

size_t ICSFile::width() const
{
    // TODO: handle order in ics
    return dims[0];
}

size_t ICSFile::height() const
{
    // TODO: handle order in ics
    return dims[1];
}

size_t ICSFile::depth() const
{
    // TODO: handle order in ics
    return dims[2];
}

std::shared_ptr<float> ICSFile::getChannelData(int channel)
{
    return channelData[channel];
}

ICSFile::~ICSFile()
{
    ICS_EC(IcsClose(ip));
}

void ICSFile::errorCheck(Ics_Error error, const std::string message)
{
    if (error != IcsErr_Ok) {
        throw ICSError(message);
    }
}

#undef ICS_EC

//------------------------------------------------------------------------------

vector<VolumetricDataPtr> VolumetricData::loadICS(string filename)
{
    vector<VolumetricDataPtr> vdList;
    ICSFile icsFile(filename);
    for (int i = 0; i < icsFile.channels(); ++i) {
        VolumetricDataPtr vd = VolumetricDataPtr::create();
        vd->m_dims[0] = icsFile.width();
        vd->m_dims[1] = icsFile.height();
        vd->m_dims[2] = icsFile.depth();
        vd->m_data = icsFile.getChannelData(i);
        emit vd->dataChanged(vd);
        vdList.push_back(vd);
    }
    return vdList;
}

VolumetricTexture::VolumetricTexture(Qt3DCore::QNode *parent)
: Qt3DRender::QAbstractTexture(QAbstractTexture::Target3D, parent)
{
    setMinificationFilter(Qt3DRender::QAbstractTexture::Filter::Linear);
    setMagnificationFilter(Qt3DRender::QAbstractTexture::Filter::Linear);
}

void VolumetricTexture::setDataSource(const VolumetricDataPtr data)
{
    setStatus(Qt3DRender::QAbstractTexture::Status::Loading);
    if (m_textureImage) {
        removeTextureImage(m_textureImage);
        delete m_textureImage;
        m_textureImage = nullptr;
    }

    m_textureImage = new VolumetricTextureImage(data);
    addTextureImage(m_textureImage);
    setStatus(Qt3DRender::QAbstractTexture::Status::Ready);
    cout << "tex ready" << endl;
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
    texImage->setDepth(m_data->depth());
    texImage->setFaces(1);
    texImage->setLayers(1);
    texImage->setMipLevels(1);
    texImage->setFormat(QOpenGLTexture::TextureFormat::R32F);
    texImage->setPixelFormat(QOpenGLTexture::PixelFormat::Red);
    texImage->setPixelType(QOpenGLTexture::PixelType::Float32);
    texImage->setTarget(QOpenGLTexture::Target::Target3D);

    const QByteArray bytes = QByteArray((char*)m_data->data().get(), m_data->sizeInBytes());
    texImage->setData(bytes, m_data->bytesPerPixel());

    return texImage;
}

bool ImageDataGenerator::operator ==(const QTextureImageDataGenerator &other) const
{
    const ImageDataGenerator *otherFunctor = functor_cast<ImageDataGenerator>(&other);
    return (otherFunctor != Q_NULLPTR && otherFunctor->m_data == m_data);
}
