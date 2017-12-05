#ifndef _volumetric_h_
#define _volumetric_h_

#undef slots
// #include <boost/python.hpp>
// #include <boost/python/numpy.hpp>

#include <QtCore>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageData>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <QQmlListProperty>
#include <QQmlComponent>
#include <QVariant>
#include <array>
#include <vector>
#include <string>
#include <exception>
#include <map>
#include <memory>
#include <functional>
#include <libics.h>

// debug:
#include <iostream>

// TODO: classes to separate files

class ICSError : public std::runtime_error {
public:
    ICSError(std::string m) : std::runtime_error(m) {}
};

class ICSFile {
public:
    ICSFile(std::string filename);
    size_t channels() const;
    size_t width() const;
    size_t height() const;
    size_t depth() const;
    std::shared_ptr<float> getChannelData(int channel);
    std::string getChannelLabel(int channel);
    ~ICSFile();

private:
    struct TypeInfoBase {
        virtual int bytes() = 0;
        virtual float cast(char *p) = 0;
    };
    typedef std::shared_ptr<TypeInfoBase> TypeInfoBasePtr;
    template <typename T> struct TypeInfo : public TypeInfoBase {
        int bytes() override { return sizeof(T); }
        float cast(char *p) override { return (float)*((T*)p); }
    };

private:
    std::string filename;
    ICS *ip = nullptr;
    Ics_DataType dt;
    int ndims = 0;
    size_t dims[ICS_MAXDIM];
    std::vector<std::shared_ptr<float>> channelData;
    std::vector<std::string> channelLabels;
    std::vector<std::string> orderLabels;
    static const std::map<Ics_DataType, TypeInfoBasePtr> typeMap;

private:
    void fillChannelData();
    void fillChannelDataWithChannelFirstDim();
    void errorCheck(Ics_Error error, const std::string message);
    template <typename T> static TypeInfoBasePtr ti() { return std::make_shared<TypeInfo<T>>(); }
};

//------------------------------------------------------------------------------

class VolumetricData;
typedef QSharedPointer<VolumetricData> VolumetricDataPtr;

class VolumetricDataManager: public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QQmlComponent::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QQmlListProperty<VolumetricData> volumes READ volumes)
public:
    typedef QQmlComponent::Status Status;
public:
    QUrl source() const { return m_source; }
    Status status() const { return m_status; }
    float progress() const { return m_progress; }
    QQmlListProperty<VolumetricData> volumes();
    void setSource(const QUrl &source);
    void setStatus(const Status &status);

    Q_INVOKABLE VolumetricData* newDataLike(VolumetricData *data, QString name);
    Q_INVOKABLE void runSegmentation(VolumetricData *data,
        VolumetricData *output, QString method, float p0, float p1);
    Q_INVOKABLE QVariantList runAnalysis(
        VolumetricData *data0,
        VolumetricData *data1,
        VolumetricData *segData0,
        VolumetricData *segData1,
        VolumetricData *output);
    Q_INVOKABLE void saveCsv(const QVariantList &list, const QStringList &heads, QUrl filename);

Q_SIGNALS:
    void sourceChanged();
    void statusChanged();
    void progressChanged();

private:
    bool loadICS(std::string filename);
    QUrl m_source;
    Status m_status = Status::Null;
    float m_progress;

private:
    struct StatRecord {
        float volume = 0.0;
        float sumIntensity = 0.0;
        float meanIntensity = 0.0;
        float overlapRatio = 0.0;
        float intersectingVolume = 0.0;
        float centerX = 0.0;
        float centerY = 0.0;
        float centerZ = 0.0;
        float intensityWeightCenterX = 0.0;
        float intensityWeightCenterY = 0.0;
        float intensityWeightCenterZ = 0.0;
    };

    void dataOpAnd(VolumetricData *data0,
        VolumetricData *data1, VolumetricData *output);
    void dataLabel(VolumetricData *data, VolumetricData *output);
 
    std::map<float, VolumetricDataManager::StatRecord>
    dataStatistics(VolumetricData *data, VolumetricData *labelData, VolumetricData *segIntersect);

    QList<VolumetricData *> m_qmlList;

// FIXME:
public:
    QVector<VolumetricDataPtr> m_dataList;
};

class Node: public QObject {
    Q_OBJECT
public:
    Node(QObject *parent = Q_NULLPTR): QObject(parent) {}
};

class SegmentationNode: public Node {
    Q_OBJECT
public:
    SegmentationNode(QObject *parent = Q_NULLPTR): Node(parent) {}
    void process(VolumetricDataPtr inputData, VolumetricDataPtr outputData, float th);
};

//------------------------------------------------------------------------------

class VolumetricData : public QObject {
    Q_OBJECT
    friend class VolumetricDataManager;

    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)
    Q_PROPERTY(int depth READ depth)
    Q_PROPERTY(QPointF dataLimits READ dataLimits)
    Q_PROPERTY(QString dataName READ dataName)
public:
    // non-copyable
    VolumetricData() { std::cout << "VolumetricData ctr " << (void*)this << std::endl; }
    ~VolumetricData() { std::cout << "VolumetricData dtr " << (void*)this << std::endl; }
    VolumetricData(const VolumetricData&) = delete;
    VolumetricData& operator=(const VolumetricData&) = delete;

    size_t width() const { return m_dims[0]; }
    size_t height() const { return m_dims[1]; }
    size_t depth() const { return m_dims[2]; }
    size_t bytesPerPixel() const { return m_bpp; }
    size_t sizeInPixels() const { return m_dims[0] * m_dims[1] * m_dims[2]; }
    size_t sizeInBytes() const { return sizeInPixels() * m_bpp; }
    const std::shared_ptr<float> data() const { return m_data; }

    // TODO: move this functionality to elsewhere this class should
    // deal with holding the data, purely
    QPointF dataLimits();
    QString dataName() const { return m_dataName; }

protected:
    std::shared_ptr<float> m_data;
    std::array<size_t, 3> m_dims = {{0, 0, 0}};
    const size_t m_bpp = sizeof(float);
    QPointF m_dataLimits;
    bool m_dataLimitsReady = false;
    QString m_dataName;
};

//------------------------------------------------------------------------------

class VolumetricTextureImage;

class VolumetricTexture : public Qt3DRender::QAbstractTexture {
    Q_OBJECT
    Q_PROPERTY(VolumetricData* data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(bool valid READ valid NOTIFY validityChanged)
public:
    explicit VolumetricTexture(Qt3DCore::QNode* parent = nullptr);
    virtual ~VolumetricTexture();

    VolumetricData* data() const { return m_data; }
    void setData(VolumetricData *data);
    bool valid() const { return m_valid; }

Q_SIGNALS:
    void validityChanged();
    void dataChanged();

protected:
    VolumetricData* m_data = nullptr;
    bool m_valid = false;
    VolumetricTextureImage* m_textureImage = nullptr;
};

//------------------------------------------------------------------------------

class ImageDataGenerator;
typedef QSharedPointer<ImageDataGenerator> ImageDataGeneratorPtr;

class VolumetricTextureImage : public Qt3DRender::QAbstractTextureImage {
    Q_OBJECT
public:
    explicit VolumetricTextureImage(const VolumetricData* data,
        Qt3DCore::QNode *parent = nullptr);
    virtual ~VolumetricTextureImage();

protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
    ImageDataGeneratorPtr m_generator;
};

//------------------------------------------------------------------------------

class ImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator {
public:
    QT3D_FUNCTOR(ImageDataGenerator)
    ImageDataGenerator(const VolumetricData* data);
    virtual ~ImageDataGenerator();
    Qt3DRender::QTextureImageDataPtr operator()() override;
    bool operator ==(const QTextureImageDataGenerator &other) const override;

private:
    const VolumetricData* m_data;
};

typedef QSharedPointer<ImageDataGenerator> DataGeneratorPtr;

#endif
