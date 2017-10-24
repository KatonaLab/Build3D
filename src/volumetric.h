#ifndef _volumetric_h_
#define _volumetric_h_

#include <QtCore>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageData>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <QQmlListProperty>
#include <QQmlComponent>
#include <array>
#include <vector>
#include <string>
#include <exception>
#include <map>
#include <functional>
#include <libics.h>

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
    static const std::map<Ics_DataType, TypeInfoBasePtr> typeMap;

private:
    void fillChannelData();
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

Q_SIGNALS:
    void sourceChanged();
    void statusChanged();
    void progressChanged();

private:
    bool loadICS(std::string filename);
    QUrl m_source;
    Status m_status = Status::Null;
    float m_progress;

// FIXME:
public:
    QVector<VolumetricDataPtr> m_dataList;
};

//------------------------------------------------------------------------------

class VolumetricData : public QObject {
    Q_OBJECT
    friend class VolumetricDataManager;
public:
    // non-copyable
    VolumetricData() = default;
    VolumetricData(const VolumetricData&) = delete;
    VolumetricData& operator=(const VolumetricData&) = delete;

    size_t width() const { return m_dims[0]; }
    size_t height() const { return m_dims[1]; }
    size_t depth() const { return m_dims[2]; }
    size_t bytesPerPixel() const { return m_bpp; }
    size_t sizeInPixels() const { return m_dims[0] * m_dims[1] * m_dims[2]; }
    size_t sizeInBytes() const { return sizeInPixels() * m_bpp; }
    const std::shared_ptr<float> data() const { return m_data; }

protected:
    std::shared_ptr<float> m_data;
    std::array<size_t, 3> m_dims = {{0, 0, 0}};
    const size_t m_bpp = sizeof(float);
};

//------------------------------------------------------------------------------

class VolumetricTextureImage;

class VolumetricTexture : public Qt3DRender::QAbstractTexture {
    Q_OBJECT
    Q_PROPERTY(const VolumetricData* data READ data WRITE setData)
    Q_PROPERTY(bool valid READ valid NOTIFY validityChanged)
public:
    explicit VolumetricTexture(Qt3DCore::QNode* parent = nullptr);
    virtual ~VolumetricTexture();

    const VolumetricData* data() const { return m_data; }
    void setData(const VolumetricData *data);
    bool valid() const { return m_valid; }

Q_SIGNALS:
    void validityChanged();

protected:
    const VolumetricData* m_data = nullptr;
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

protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
    ImageDataGeneratorPtr m_generator;
};

//------------------------------------------------------------------------------

class ImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator {
public:
    QT3D_FUNCTOR(ImageDataGenerator)
    ImageDataGenerator(const VolumetricData* data);
    Qt3DRender::QTextureImageDataPtr operator()() override;
    bool operator ==(const QTextureImageDataGenerator &other) const override;

private:
    const VolumetricData* m_data;
};

typedef QSharedPointer<ImageDataGenerator> DataGeneratorPtr;

#endif
