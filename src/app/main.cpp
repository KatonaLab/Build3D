#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <QApplication>
#include <QtGlobal>
#include <QOpenGLContext>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QCommandLineParser>

#include "client/crashpad_client.h"
#include "client/crash_report_database.h"
#include "client/settings.h"

#include "version.h"
#include "LogCollector.h"
#include "VolumeData.h"
#include "VolumeTexture.h"
#include "ModulePlatformBackend.h"
#include "TurnTableCameraController.h"
#include "BackendStore.h"
#include "BackendOutput.h"
#include "GlobalSettings.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif


using namespace std;
using namespace crashpad;

static bool startCrashHandler()
{
    map<string, string> annotations;
    vector<string> arguments;
    CrashpadClient client;
    bool rc;
    
#ifdef _WIN32
    wchar_t exePathChar[2048];
    wstring exePath(exePathChar, GetModuleFileName(NULL, exePathChar, 2048));
    exePath = exePath.substr(0, exePath.rfind('\\'));
    // TODO: change it to AppData since Program Files is readonly for the application
    wstring db_path(exePath + L"\\crashes");
    wstring handler_path(exePath + L"\\crashpad_handler.exe");
    wcout << handler_path << endl;
#else
    string db_path("crashes/");
    string handler_path("./crashpad_handler");
#endif

    string url("https://a3dc.sp.backtrace.io:6098");
    annotations["token"] = "e8c10c5d9cd420229c8d21a7d6c365ea88a4dae0d79bc2cc8c4623b851d8bf02";
    annotations["format"] = "minidump";

    base::FilePath db(db_path);
    base::FilePath handler(handler_path);
    arguments.push_back("--no-rate-limit");

    unique_ptr<CrashReportDatabase> database =
        crashpad::CrashReportDatabase::Initialize(db);

    if (database == nullptr || database->GetSettings() == NULL)
        return false;

    database->GetSettings()->SetUploadsEnabled(true);

    rc = client.StartHandler(handler, db, db, url, annotations, arguments, true, false);

    return rc;
}

void setSurfaceFormat()
{
    QSurfaceFormat format;

    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }

    format.setDepthBufferSize(24);
    format.setSamples(0);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char* argv[])
{
    // ensure logging is alive from the first moment
    LogCollector::instance();

    // FIXME: trial period has expired at bactrace.io so no minidump upload can be done
    // if (startCrashHandler() == false) {
    //     cout << "crash reporter could not start" << endl;
    //     return -1;
    // }

    qputenv("QT_STYLE_OVERRIDE", "Material");
    QApplication::setStyle(QStyleFactory::create("Material"));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    app.setOrganizationName("MTA KOKI KatonaLab");
    app.setOrganizationDomain("koki.hu");
    app.setApplicationName("A3DC (" + QString(_A3DC_BUILD_GIT_SHA) + QString(_A3DC_BUILD_PLATFORM) + ")");
    // TODO: call setApplicationVersion

    QCommandLineParser parser;
    QCommandLineOption pythonPathOption("python-path", "sets PYTHONHOME", "PY_PATH");
    parser.addOption(pythonPathOption);
    QCommandLineOption modulePathOption("module-path", "sets the module path", "MODULE_PATH");
    parser.addOption(modulePathOption);
    QCommandLineOption editorOption("editor", "opens the program in editor mode");
    parser.addOption(editorOption);
    parser.process(app);

    QString pythonPath = parser.value(pythonPathOption);
    QString modulePath = parser.value(modulePathOption);
    GlobalSettings::editorMode = parser.isSet(editorOption);

    if (pythonPath.isEmpty()) {
        QByteArray venv = qgetenv("VIRTUAL_ENV");
        if (venv.isEmpty()) {
            pythonPath = QString("virtualenv");
        } else {
            pythonPath = QString::fromLocal8Bit(venv);
        }
    }
    qputenv("PYTHONHOME", pythonPath.toLocal8Bit());

    if (!modulePath.isEmpty()) {
        GlobalSettings::modulePath = modulePath;
    } else {
        GlobalSettings::modulePath = QString("modules");
    }

    qInfo() << "PYTHONHOME:" << pythonPath;
    qInfo() << "module path:" << GlobalSettings::modulePath.dirName();
    if (GlobalSettings::editorMode) {
        qInfo() << "editor mode";
    }

    setSurfaceFormat();

    qmlRegisterInterface<ImageOutputValue>("ImageOutputValue");
    qmlRegisterType<VolumeTexture>("koki.katonalab.a3dc", 1, 0, "VolumeTexture");
    qmlRegisterType<TurnTableCameraController>("koki.katonalab.a3dc", 1, 0, "TurnTableCameraController");
    qmlRegisterSingletonType<A3DCVersion>("koki.katonalab.a3dc", 1, 0, "A3DCVersion", singletonA3DCVersionProvider);
    // NOTE: it will initialize LogCollector and routes the all qDebug/qInfo... log through this instance
    // see LogCollector.cpp for details
    qmlRegisterSingletonType<LogCollector>("koki.katonalab.a3dc", 1, 0, "LogCollector", singletonLogCollectorProvider);

    qmlRegisterInterface<BackendStoreItem>("BackendStoreItem");
    qmlRegisterType<BackendStore>("koki.katonalab.a3dc", 1, 0, "BackendStore");
    qmlRegisterType<BackendStoreFilter>("koki.katonalab.a3dc", 1, 0, "BackendStoreFilter");

    if (QFontDatabase::addApplicationFont(":/assets/fonts/fontello.ttf") == -1) {
        qWarning() << "Failed to load fontello.ttf";
    }

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}
