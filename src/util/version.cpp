#include "version.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const char * _A3DC_BUILD_DATE = TOSTRING(DEFINED_AT_COMPILATION_A3DC_BUILD_DATE);
const char * _A3DC_BUILD_GIT_SHA = TOSTRING(DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA);
const char * _A3DC_BUILD_MODE = TOSTRING(DEFINED_AT_COMPILATION_A3DC_BUILD_MODE);
const char * _A3DC_BUILD_PLATFORM = TOSTRING(DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM);

A3DCVersion::A3DCVersion(QObject* parent)
    : QObject(parent)
{
    m_version = QString(QString(_A3DC_BUILD_GIT_SHA) + "@" + QString(_A3DC_BUILD_PLATFORM) + "@" + QString(_A3DC_BUILD_DATE) + "@" + QString(_A3DC_BUILD_MODE));
}

QObject* singletonA3DCVersionProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    // NOTE: the instance is owned by the QML engine, so destruction is handled
    A3DCVersion *theOne = new A3DCVersion;
    return theOne;
}