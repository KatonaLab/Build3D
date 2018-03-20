#ifndef _app_VolumeDataCollection_h_
#define _app_VolumeDataCollection_h_

#include <QtCore>
#include <QQmlComponent>

#include <core/multidim_image_platform/MultiDimImage.hpp>

class VolumeTexture;
typedef QSharedPointer<VolumeTexture> VolumeTexturePtr;

class VolumeDataCollection: public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QQmlComponent::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QQmlListProperty<VolumeTexture> volumes READ volumes)
public:
    typedef QQmlComponent::Status Status;
public:
    explicit VolumeDataCollection(QObject *parent = Q_NULLPTR);
    QUrl source() const { return m_source; }
    Status status() const { return m_status; }
    float progress() const { return m_progress; }
    QQmlListProperty<VolumeTexture> volumes();
    void setSource(const QUrl &source);
    void setStatus(const Status &status);
    virtual ~VolumeDataCollection() = default;
Q_SIGNALS:
    void sourceChanged();
    void statusChanged();
    void progressChanged();
private:
    bool loadICS(const std::string& filename);
    core::multidim_image_platform::MultiDimImage<float> m_image;
    QUrl m_source;
    Status m_status = Status::Null;
    float m_progress;
    QList<VolumeTexture *> m_qmlList;
    QVector<VolumeTexturePtr> m_dataList;
};

#endif
