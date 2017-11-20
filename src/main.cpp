#include <iostream>
//#include <python/Python.h>
//#include <numpy/arrayobject.h>

#include <string>

// #ifdef QT_STATICPLUGIN
    #include <QtPlugin>
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
// #endif

#include <QApplication>
//#include <QVector>
#include <QQuickView>
#include <QStyleFactory>

#include <QOpenGLContext>
#include <Qt3DInput/QInputSettings>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

//#include <Qt3DRender/QTexture>
//#include <Qt3DRender/QTextureImage>
//#include <Qt3DRender/QTextureImageData>
//#include <Qt3DRender/QTextureImageDataGenerator>

#include <QQmlDebuggingEnabler>

#include "volumetric.h"

//#include <functional>
//#include <array>

using namespace std;

//VolumetricDataPtr filterDemo(VolumetricDataPtr vd);
//PyObject* vdToList(VolumetricDataPtr vd);

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
//    QQmlDebuggingEnabler enabler;

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    app.setOrganizationName("MTA KOKI KatonaLab");
    app.setOrganizationDomain("koki.hu");
    app.setApplicationName("A3DC");

    setSurfaceFormat();

    qmlRegisterType<VolumetricData>("koki.katonalab.a3dc", 1, 0, "VolumetricData");
    qmlRegisterType<VolumetricDataManager>("koki.katonalab.a3dc", 1, 0, "VolumetricDataManager");
    qmlRegisterType<VolumetricTexture>("koki.katonalab.a3dc", 1, 0, "VolumetricTexture");

    if (QFontDatabase::addApplicationFont(":/assets/fonts/fontello.ttf") == -1) {
        qWarning() << "Failed to load fontello.ttf";
    }

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

//    if (engine.rootObjects().isEmpty()) {
//        cerr << "qml error" << endl;
//        return -1;
//    }
//
////    "/Users/fodorbalint/projects/a3dc/example/K32_bassoon_TH_vGluT1_c01_cmle.ics"
//    vector<VolumetricDataPtr> dataVec = VolumetricData::loadICS("/Users/fodorbalint/Desktop/test.ics");
//
//    VolumetricDataManager dm;
//    dm.setSource(QUrl("file:///Users/fodorbalint/Desktop/K32_bassoon_TH_vGluT1_c01_cmle.ics"));
//
//    QList<QObject*> roots = engine.rootObjects();
//    VolumetricTexture *tex = roots[0]->findChild<VolumetricTexture*>("objVol");
//    tex->setData(dm.m_dataList[0].data());

    return app.exec();

//    QQuickView view;
//    view.resize(800, 800);
//    view.setResizeMode(QQuickView::SizeRootObjectToView);
//    view.setSource(QUrl("qml/main.qml"));
//    view.show();

//    Qt3DInput::QInputSettings *inputSettings = view.findChild<Qt3DInput::QInputSettings *>();
//    if (inputSettings) {
//        inputSettings->setEventSource(&view);
//    } else {
//        cerr << "No Input Settings found, keyboard and mouse events won't be handled";
//    }

//    return app.exec();
}

//void pythonTest()
//{
//    Py_SetProgramName((char *)"myPythonProgram");
//    Py_Initialize();
//    // parDict is a parameter to send to python function
//    PyObject * parDict;
//    parDict = PyDict_New();
//    PyDict_SetItemString(parDict, "x0", PyFloat_FromDouble(1.0));
//    // run python code to load functions
//    PyRun_SimpleString("exec(open('scripts/cpptest.py').read())");
//    // get function showval from __main__
//    PyObject* main_module = PyImport_AddModule("__main__");
//    PyObject* global_dict = PyModule_GetDict(main_module);
//    PyObject* func = PyDict_GetItemString(global_dict, "showval");
//    // parameter should be a tuple
//    PyObject* par = PyTuple_Pack(1, parDict);
//    // call the function
//    PyObject_CallObject(func, par);
//    Py_Finalize();
//}

//void init_numpy()
//{
//    import_array();
//}

//PyObject* vdToList(VolumetricDataPtr vd)
//{
//    size_t n = vd->getWidth() * vd->getHeight();
//    PyObject* listObj = PyList_New(n);
//    if (!listObj) throw logic_error("Unable to allocate memory for Python list");
//    for (unsigned int i = 0; i < n; i++) {
//        PyObject *num = PyFloat_FromDouble((double) vd->getConstData()[i]);
//        if (!num) {
//            Py_DECREF(listObj);
//            throw logic_error("Unable to allocate memory for Python list");
//        }
//        PyList_SET_ITEM(listObj, i, num);
//    }
//    return listObj;
//}
//
//void listToVd(PyObject* incoming, VolumetricDataPtr vd)
//{
//    if (PyList_Check(incoming)) {
//        for (Py_ssize_t i = 0; i < PyList_Size(incoming); i++) {
//            PyObject *value = PyList_GetItem(incoming, i);
//            vd->getData()[i] = (uchar)PyFloat_AsDouble(value);
//        }
//    } else {
//        cerr << "py return is not a list" << endl;
//    }
//}
//
//VolumetricDataPtr filterDemo(VolumetricDataPtr vd)
//{
//    Py_SetProgramName((char*)"myPythonProgram");
//    Py_Initialize();
//    // parDict is a parameter to send to python function
//    PyObject * parDict;
//    parDict = PyDict_New();
//    PyDict_SetItemString(parDict, "data", vdToList(vd));
//    PyDict_SetItemString(parDict, "width", PyFloat_FromDouble(vd->getWidth()));
//    PyDict_SetItemString(parDict, "height", PyFloat_FromDouble(vd->getHeight()));
//    // run python code to load functions
//    PyRun_SimpleString("exec(open('scripts/cpptest.py').read())");
//    // get function showval from __main__
//    PyObject* main_module = PyImport_AddModule("__main__");
//    PyObject* global_dict = PyModule_GetDict(main_module);
//    PyObject* func = PyDict_GetItemString(global_dict, "filter");
//    // parameter should be a tuple
//    PyObject* par = PyTuple_Pack(1, parDict);
//    // call the function
//    PyObject *pValue;
//    pValue = PyObject_CallObject(func, par);
//    listToVd(pValue, vd);
//    Py_Finalize();
//
//    return vd;
//}

