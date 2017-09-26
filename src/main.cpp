#include <iostream>
#include <python/Python.h>
#include <libics.h>

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <QGuiApplication>
#include <QVector>
#include <QQuickView>
#include <QtDataVisualization/QCustom3DVolume.h>

#include <QQuickView>
#include <QOpenGLContext>

using namespace std;

using namespace QtDataVisualization;

void pythonTest();
void icsTest();

void setSurfaceFormat()
{
    QSurfaceFormat format;

    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }

    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char* argv[])
{
    // pythonTest();
    QGuiApplication app(argc, argv);

    setSurfaceFormat();

    // Qt3DExtras::Quick::Qt3DQuickWindow view;
    
    QQuickView view;

    view.resize(1024, 1024);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("main.qml"));
    view.show();



    // QQuickView view;
    // view.setSource(QUrl("main.qml"));
    // view.show();

    QQuickItem *object = view.rootObject();
    // QCustom3DVolume *cv = object->findChild<QCustom3DVolume*>("volumeView");
    // if (rect) {
    //     rect->setProperty("color", "red");
    // }

    return app.exec();
}

void pythonTest()
{
    Py_SetProgramName("myPythonProgram");
    Py_Initialize();
    // parDict is a parameter to send to python function
    PyObject * parDict;
    parDict = PyDict_New();
    PyDict_SetItemString(parDict, "x0", PyFloat_FromDouble(1.0));
    // run python code to load functions
    PyRun_SimpleString("exec(open('cpptest.py').read())");
    // get function showval from __main__
    PyObject* main_module = PyImport_AddModule("__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* func = PyDict_GetItemString(global_dict, "showval");
    // parameter should be a tuple
    PyObject* par = PyTuple_Pack(1, parDict);
    // call the function
    PyObject_CallObject(func, par);
    Py_Finalize();
}

QVector<uchar> loadICS()
{
    ICS* ip;
    Ics_DataType dt;
    int ndims;
    size_t dims[ICS_MAXDIM];
    size_t bufsize;
    void* buf;
    Ics_Error retval;

    retval = IcsOpen(&ip, "K32_bassoon_TH_vGluT1_c01_cmle.ics", "r");
    if (retval != IcsErr_Ok) {
       cerr << "err IcsOpen" << endl;
    }

    IcsGetLayout(ip, &dt, &ndims, dims);
    bufsize = IcsGetDataSize (ip);
    buf = malloc(bufsize);
    if (buf == NULL){
       cerr << "err malloc" << endl;
    }
    cout << "ndims " << ndims << endl;
    for (int i = 0; i < ndims; ++i) {
        cout << dims[i] << endl;
    }

    retval = IcsGetData(ip, buf, bufsize);
    if (retval != IcsErr_Ok) {
       cerr << "err IcsGetData" << endl;
    }

    /*
     * There are some other functions available to get
     * more info on the image in the .ics file.
     */

    retval = IcsClose(ip);
    if (retval != IcsErr_Ok) {
       cerr << "err IcsClose" << endl;
    }
}
