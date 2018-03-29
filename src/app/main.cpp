#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <QApplication>
#include <QOpenGLContext>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#include "client/crashpad_client.h"
#include "client/crash_report_database.h"
#include "client/settings.h"

#include "version.h"
// #include "volumetric.h"

#include "VolumeData.h"
#include "VolumeDataCollection.h"
#include "VolumeTexture.h"

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
    // modify it to appdata
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
    // if (startCrashHandler() == false) {
    //     cout << "crash reporter could not start" << endl;
    //     return -1;
    // }

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    app.setOrganizationName("MTA KOKI KatonaLab");
    app.setOrganizationDomain("koki.hu");
    app.setApplicationName("A3DC (" + QString(_A3DC_BUILD_GIT_SHA) + QString(_A3DC_BUILD_PLATFORM) + ")");

    setSurfaceFormat();

    qmlRegisterType<VolumeDataCollection>("koki.katonalab.a3dc", 1, 0, "VolumeDataCollection");
    qmlRegisterType<VolumeTexture>("koki.katonalab.a3dc", 1, 0, "VolumeTexture");
    qmlRegisterSingletonType<A3DCVersion>("koki.katonalab.a3dc", 1, 0, "A3DCVersion", singletonA3DCVersionProvider);

    if (QFontDatabase::addApplicationFont(":/assets/fonts/fontello.ttf") == -1) {
        qWarning() << "Failed to load fontello.ttf";
    }

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}