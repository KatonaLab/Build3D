#ifndef _volumetric_h_
#define _volumetric_h_

#include <QtCore>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageData>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <array>
#include <vector>
#include <string>

class VolumetricData;
typedef QSharedPointer<VolumetricData> VolumetricDataPtr;

class VolumetricData : public QObject {
    Q_OBJECT
public:
    // non-copyable
    VolumetricData() = default;
    VolumetricData(const VolumetricData&) = delete;
    VolumetricData& operator=(const VolumetricData&) = delete;

    virtual ~VolumetricData() { delete [] m_data; }
    size_t width() const { return m_dims[0]; }
    size_t height() const { return m_dims[1]; }
    size_t depth() const { return m_dims[2]; }
    size_t bytesPerPixel() const { return m_bpp; }
    size_t sizeInPixels() const { return m_dims[0] * m_dims[1] * m_dims[2]; }
    size_t sizeInBytes() const { return sizeInPixels() * m_bpp; }
    const char* data() const { return m_data; }

Q_SIGNALS:
    void dataChanged(const VolumetricDataPtr data);

protected:
    char *m_data = nullptr;
    std::array<size_t, 3> m_dims = {{0, 0, 0}};
    size_t m_bpp = 0;

public:
    static std::vector<VolumetricDataPtr> loadICS(std::string filename);
};

//------------------------------------------------------------------------------

class VolumetricTextureImage;

class VolumetricTexture : public Qt3DRender::QAbstractTexture {
    Q_OBJECT
public:
    explicit VolumetricTexture(Qt3DCore::QNode *parent = nullptr);
    virtual ~VolumetricTexture();
public Q_SLOTS:
    void setDataSource(const VolumetricDataPtr data);

protected:
    VolumetricTextureImage *m_textureImage = nullptr;
};

//------------------------------------------------------------------------------

class ImageDataGenerator;
typedef QSharedPointer<ImageDataGenerator> ImageDataGeneratorPtr;

class VolumetricTextureImage : public Qt3DRender::QAbstractTextureImage {
    Q_OBJECT
public:
    explicit VolumetricTextureImage(const VolumetricDataPtr data,
                                    Qt3DCore::QNode *parent = nullptr);

protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
    ImageDataGeneratorPtr m_generator;
};

//------------------------------------------------------------------------------

class ImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator {
public:
    QT3D_FUNCTOR(ImageDataGenerator)
    ImageDataGenerator(VolumetricDataPtr data);
    Qt3DRender::QTextureImageDataPtr operator()() override;
    bool operator ==(const QTextureImageDataGenerator &other) const override;
private:
    VolumetricDataPtr m_data;
};

typedef QSharedPointer<ImageDataGenerator> DataGeneratorPtr;

#endif
