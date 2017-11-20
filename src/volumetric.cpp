#include "volumetric.h"

#include <iostream>

#include <QTextureWrapMode>
#include <QPointF>
#include <QQmlEngine>

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
    // TODO: rethink, potential leak
     vd->setParent(this); // prevent from deleting the object

    // Hope this fix issue #11
    QQmlEngine::setObjectOwnership(vd.data(), QQmlEngine::CppOwnership);

    vd->m_dims = data->m_dims;

    // TOOD: there is some unknown crash that points to VolumetricData destructor:
    // app(49302,0x7fffa985e340) malloc: *** error for object 0x1222af510: pointer being freed was not allocated
    // it might originate here
    // anyway this whole mess with the VolumetricData handling should be refactored
    // keeping thread safety and flexibility in mind

    vd->m_data.reset(new float[vd->sizeInPixels()], default_delete<float[]>());
    vd->m_dataName = name;
    vd->m_dataLimitsReady = false;
    // TODO: should I append?
    m_dataList.append(vd);
    return vd.data();
}

void VolumetricDataManager::runSegmentation(VolumetricData *data, 
        VolumetricData *output, QString method, float p0, float p1)
{
    if (data->sizeInPixels() != output->sizeInPixels()) {
        // TODO throw error
        cerr << "runSegmentation: input-output size mismatch" << endl;
        return;
    }
    float scale = 1. / data->dataLimits().y();
    for (int i = 0; i < data->sizeInPixels(); ++i) {
        float v = data->m_data.get()[i] * scale;
        output->m_data.get()[i] = (p0 <= v && v <= p1) ? 1. : 0.;
    }
    output->m_dataLimitsReady = false;
}

void VolumetricDataManager::runAnalysis(
    VolumetricData *data0,
    VolumetricData *data1, 
    VolumetricData *segData0,
    VolumetricData *segData1,
    VolumetricData *output)
{
    // TODO: size check
    cout << "analysis 1" << endl;
    VolumetricData* label0 = newDataLike(segData0, "");
    cout << "analysis 2" << endl;
    VolumetricData* label1 = newDataLike(segData1, "");
    cout << "analysis 3" << endl;
    dataLabel(segData0, label0);
    cout << "analysis 4" << endl;
    dataLabel(segData1, label1);

//    float minVolume0 = 0.0;
//    float maxVolume0 = 100.0;
//    float minVolume1 = 0.0;
//    float maxVolume1 = 100.0;

//    dataFilter(tmp0, data0, tmp0, minVolume0, maxVolume0);
//    dataFilter(tmp1, data1, tmp1, minVolume1, maxVolume1);

    VolumetricData* intersect = newDataLike(segData1, "");
    cout << "analysis 5" << endl;
    dataOpAnd(segData0, segData1, intersect);
    cout << "analysis 6" << endl;
    VolumetricData* intersectLabel = newDataLike(segData1, "");
    cout << "analysis 7" << endl;
    dataLabel(intersect, output);
    cout << "analysis 8" << endl;

//    dataFilter(output, )

    auto statList0 = dataStatistics(data0, label0, intersect);
    cout << "analysis 9" << endl;
    auto statList1 = dataStatistics(data1, label1, intersect);
    cout << "analysis 10" << endl;

    output->m_dataLimitsReady = false;
}

void VolumetricDataManager::dataOpAnd(VolumetricData *data0, VolumetricData *data1,
    VolumetricData *output)
{
    // TODO: size check
    
    for (int i = 0; i < data0->sizeInPixels(); ++i) {
        bool v0 = data0->m_data.get()[i] != 0.0f;
        bool v1 = data1->m_data.get()[i] != 0.0f;
        output->m_data.get()[i] = v0 && v1 ? 1.0 : 0.0;
    }
    output->m_dataLimitsReady = false;
}

std::map<float, VolumetricDataManager::StatRecord>
VolumetricDataManager::dataStatistics(VolumetricData *data,
                                           VolumetricData *labelData,
                                           VolumetricData *segIntersect)
{
    // TODO: size check

    map<float, StatRecord> statMap;

    for (int i = 0; i < data->sizeInPixels(); ++i) {
        float value = data->m_data.get()[i];
        float label = labelData->m_data.get()[i];
        float coloc = segIntersect->m_data.get()[i];
        if (label != 0.0f) {
            statMap[label].volume++;
            statMap[label].intensity += value;
            if (coloc != 0.0f) {
                statMap[label].intersectVolume++;
            }
        }
    }
    for (auto &item : statMap) {
        item.second.intensity /= item.second.volume;
        item.second.overlapRatio = item.second.intersectVolume / item.second.volume;
        cout << item.first << ": "
            << item.second.intensity << " "
            << item.second.volume << " "
            << item.second.overlapRatio << endl;
    }
    return statMap;
}

void VolumetricDataManager::dataLabel(VolumetricData *data, VolumetricData *output)
{
    // TODO: size check

    // TODO: find a better way
    for (int i = 0; i < output->sizeInPixels(); ++i) {
        output->m_data.get()[i] = 0.0f;
    }

    // TODO: refactor
    auto getIndex = [&data](int x, int y, int z)
    {
        return x * (data->height() * data->depth()) + y * (data->depth()) + z;
    };

    float nextLabel = 1.0f;
    auto getNearLabel = [&output, &getIndex](int x, int y, int z)
    {
        for (int u = -1; u <= 0; ++u) {
            for (int v = -1; v <= 0; ++v) {
                for (int w = -1; w <= 0; ++w) {
                    int idx = getIndex(x + u, y + v, z + w);
                    if (output->m_data.get()[idx] > 0.0) {
                        return output->m_data.get()[idx];
                    }
                }
            }
        }
        return 0.0f;
    };

    // connected components first pass

    for (int x = 1; x < data->width(); ++x) {
        for (int y = 1; y < data->height(); ++y) {
            for (int z = 1; z < data->depth(); ++z) {
                int idx = getIndex(x, y, z);
                if (data->m_data.get()[idx] > 0.0f) {
                    float label = getNearLabel(x, y, z);
                    output->m_data.get()[idx] = label > 0.0f ? label : nextLabel++;
                }
            }
        }
    }

    // connected components second pass

    float maxLabel = 0.0f;

    for (int x = 1; x < data->width()-1; ++x) {
        for (int y = 1; y < data->height()-1; ++y) {
            for (int z = 1; z < data->depth()-1; ++z) {
                int idx = getIndex(x, y, z);
                float label = output->m_data.get()[idx];
                if (label > 0.0f) {
                    float newLabel = min(label, getNearLabel(x, y, z));
                    output->m_data.get()[idx] = newLabel;
                    maxLabel = max(maxLabel, newLabel);
                }
            }
        }
    }
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
    cout << "VolumetricTexture ctr " << (void*)this << endl;
    setMinificationFilter(Qt3DRender::QAbstractTexture::Filter::Nearest);
    setMagnificationFilter(Qt3DRender::QAbstractTexture::Filter::Nearest);
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
    m_textureImage = new VolumetricTextureImage(m_data, this);
    addTextureImage(m_textureImage);
    setStatus(Qt3DRender::QAbstractTexture::Status::Ready);

    m_valid = true;
    emit validityChanged();
}

VolumetricTexture::~VolumetricTexture()
{
    delete m_textureImage;
    cout << "VolumetricTexture dtr " << (void*)this << endl;
}

//------------------------------------------------------------------------------

VolumetricTextureImage::VolumetricTextureImage(const VolumetricData* data, QNode *parent)
: QAbstractTextureImage(parent)
{
    cout << "VolumetricTextureImage ctr " << (void*)this << endl;
    m_generator = ImageDataGeneratorPtr::create(data);
}

QTextureImageDataGeneratorPtr VolumetricTextureImage::dataGenerator() const
{
    return m_generator;
}

VolumetricTextureImage::~VolumetricTextureImage()
{
    cout << "VolumetricTextureImage dtr " << (void*)this << endl;
}

//------------------------------------------------------------------------------

ImageDataGenerator::ImageDataGenerator(const VolumetricData* data) : m_data(data)
{
    cout << "ImageDataGenerator ctr " << (void*)this << endl;
}

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

ImageDataGenerator::~ImageDataGenerator()
{
    cout << "ImageDataGenerator dtr " << (void*)this << endl;
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
