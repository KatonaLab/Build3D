#include "volumetric.h"

#include <iostream>

#include <QTextureWrapMode>
#include <QPointF>

// TEST
#include <chrono>
#include <thread>
#include <algorithm>

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

    for (int i = 0; i < ndims; ++i) {
        char order[ICS_STRLEN_TOKEN];
        char label[ICS_STRLEN_TOKEN];
        ICS_EC(IcsGetOrder(ip, i, order, label));
        channelLabels.push_back(label);
   }

    fillChannelData();
}

std::string ICSFile::getChannelLabel(int channel)
{
    return channelLabels[channel];
}

void ICSFile::fillChannelData()
{
    size_t size = IcsGetDataSize(ip);
    shared_ptr<char> buffer(new char[size], default_delete<char[]>());
    ICS_EC(IcsGetData(ip, buffer.get(), size));

    size_t n = width() * height() * depth();
    size_t c = channels();
    for (size_t k = 0; k < c; ++k) {
        // TODO: consider converting to int32/64 instead of float
        channelData.emplace_back(new float[n], default_delete<float[]>());
    }

    auto cast = bind(&TypeInfoBase::cast, typeMap.at(dt), std::placeholders::_1);
    size_t bytes = typeMap.at(dt)->bytes();
    for (int k = 0; k < c; ++k) {
        size_t offset = n * k;
        for (size_t i = 0; i < n; ++i) {
            channelData[k].get()[i] = cast(buffer.get() + (i + offset) * bytes);
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

QPointF VolumetricData::dataLimits()
{
    if (!m_dataLimitsReady && sizeInPixels()) {
        auto mm = std::minmax_element(m_data.get(), m_data.get() + sizeInPixels());
        m_dataLimits = QPointF(*mm.first, *mm.second);
    }
    return m_dataLimits;
}

//------------------------------------------------------------------------------

bool VolumetricDataManager::loadICS(string filename)
{
    m_progress = 0.0f;
    emit progressChanged();

    m_dataList.clear();
    try {
        ICSFile icsFile(filename);
        for (int i = 0; i < icsFile.channels(); ++i) {
            VolumetricDataPtr vd = VolumetricDataPtr::create();
            vd->m_dims[0] = icsFile.width();
            vd->m_dims[1] = icsFile.height();
            vd->m_dims[2] = icsFile.depth();
            vd->m_data = icsFile.getChannelData(i);
            vd->m_dataName = QString::fromStdString(icsFile.getChannelLabel(i));
            m_dataList.append(vd);

            m_progress = (i + 1) / (float)icsFile.channels();
            emit progressChanged();
        }
    } catch (ICSError &e) {
        // FIXME
        cout << e.what() << endl;
        return false;
    }
    return true;
}

void VolumetricDataManager::setSource(const QUrl& source)
{
    // TODO: loading to another thread (?)
    m_source = source;
    emit sourceChanged();
    setStatus(Status::Loading);
    bool success = loadICS(source.toLocalFile().toStdString());
    setStatus(success ? Status::Ready : Status::Error);
}

void VolumetricDataManager::setStatus(const Status &status)
{
    m_status = status;
    emit statusChanged();
}

VolumetricData* VolumetricDataManager::newDataLike(VolumetricData *data, QString name)
{
    VolumetricDataPtr vd = VolumetricDataPtr::create();
    vd->m_dims = data->m_dims;
    shared_ptr<float> buffer(new float[vd->sizeInPixels()], default_delete<float[]>());
    for (int i = 0; i < vd->sizeInPixels(); ++i) {
        buffer.get()[i] = sin(i);
    }
    vd->m_data = buffer;
    vd->m_dataName = name;
    m_dataList.append(vd);
    return vd.data();
}

void VolumetricDataManager::runSegmentation(VolumetricData *data, 
        VolumetricData *output, QString method, float p0, float p1)
{
    cout << "VolumetricDataManager::runSegmentation called " 
    << (void*)data << ", "
    << (void*)output << ", "
    << p0 << ", "
    << p1 << endl;
}

QQmlListProperty<VolumetricData> VolumetricDataManager::volumes()
{
    QList<VolumetricData *> list;
    for (auto &elem : m_dataList) {
        list.append(elem.data());
    }
    return QQmlListProperty<VolumetricData>(this, list);
}
//------------------------------------------------------------------------------

VolumetricTexture::VolumetricTexture(Qt3DCore::QNode *parent)
: Qt3DRender::QAbstractTexture(QAbstractTexture::Target3D, parent)
{
    setMinificationFilter(Qt3DRender::QAbstractTexture::Filter::Linear);
    setMagnificationFilter(Qt3DRender::QAbstractTexture::Filter::Linear);
    setWrapMode(Qt3DRender::QTextureWrapMode(QTextureWrapMode::ClampToBorder));
}

void VolumetricTexture::setData(VolumetricData* data)
{
    setStatus(Qt3DRender::QAbstractTexture::Status::Loading);
    if (m_textureImage) {
        removeTextureImage(m_textureImage);
        delete m_textureImage;
        m_textureImage = nullptr;
    }

    m_data = data;
    m_textureImage = new VolumetricTextureImage(m_data);
    addTextureImage(m_textureImage);
    setStatus(Qt3DRender::QAbstractTexture::Status::Ready);

    m_valid = true;
    emit validityChanged();
}

VolumetricTexture::~VolumetricTexture()
{
    delete m_textureImage;
}

//------------------------------------------------------------------------------

VolumetricTextureImage::VolumetricTextureImage(const VolumetricData* data, QNode *parent)
: QAbstractTextureImage(parent)
{
    m_generator = ImageDataGeneratorPtr::create(data);
}

QTextureImageDataGeneratorPtr VolumetricTextureImage::dataGenerator() const
{
    return m_generator;
}

//------------------------------------------------------------------------------

ImageDataGenerator::ImageDataGenerator(const VolumetricData* data) : m_data(data)
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

//------------------------------------------------------------------------------

void SegmentationNode::process(VolumetricDataPtr inputData, VolumetricDataPtr outputData, float th)
{
    // namespace p = boost::python;
    // namespace np = boost::python::numpy;

    // Py_Initialize();
    // np::initialize();

    // np::ndarray inputArray = np::from_data(inputData->data().get(),
    //                                        np::dtype::get_builtin<float>(),
    //                                        p::make_tuple(inputData->width(), inputData->height(), inputData->depth()),
    //                                        p::make_tuple(inputData->height()*inputData->depth()*sizeof(float), inputData->depth()*sizeof(float), sizeof(float)),
    //                                        p::object()
    //                                      );

}
