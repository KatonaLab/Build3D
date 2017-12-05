#ifndef _version_h_
#define _version_h_

// extern const char * _A3DC_BUILD_DATE;
extern const char * _A3DC_BUILD_GIT_SHA;
extern const char * _A3DC_BUILD_PLATFORM;

#include <QtCore>
#include <QQmlEngine>
#include <QJSEngine>

class A3DCVersion : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString version READ version)
public:
    explicit A3DCVersion(QObject* parent = nullptr);
    QString version() { return m_version; }
private:
    QString m_version;
};

QObject* singletonA3DCVersionProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

#endif // _version_h_