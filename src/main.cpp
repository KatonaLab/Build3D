#include <iostream>
#include <python/Python.h>
#include <numpy/arrayobject.h>

#include <libics.h>
#include <string>

#include <QGuiApplication>
#include <QVector>
#include <QQuickView>

#include <QOpenGLContext>

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureImage>
#include <Qt3DRender/QTextureImageData>
#include <Qt3DRender/QTextureImageDataGenerator>

#include "VolumeMaterial.h"

#include <functional>
#include <array>

using namespace std;

class VolumetricData;
typedef shared_ptr<VolumetricData> VolumetricDataPtr;

class VolumetricData {
public:
    VolumetricData() : dims{{0, 0, 0}}, data(nullptr) {}
    virtual ~VolumetricData() {}
    const array<size_t, 3>& size() { return dims; }
    const uchar* getConstData() { return data; }
    uchar* getData() { return data; }
    size_t getWidth() { return dims[0]; }
    size_t getHeight() { return dims[1]; }
    size_t getDepth() { return dims[2]; }
    // TODO: make it non-copyable or impl the big 3/4/5
protected:
    array<size_t, 3> dims;
    uchar *data;
public:
    static vector<VolumetricDataPtr> loadICS(string filename);
};

VolumetricDataPtr filterDemo(VolumetricDataPtr vd);
PyObject* vdToList(VolumetricDataPtr vd);
//void pythonTest();
//uchar* loadICSPreview();

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
    QGuiApplication app(argc, argv);
    setSurfaceFormat();

    qmlRegisterType<VolumeMaterial>("foo.bar", 1, 0, "VolumeMaterial");

    QQuickView view;
    view.resize(800, 800);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qml/main.qml"));
    view.show();

//    Qt3DRender::QTextureImage *texImage = new Qt3DRender::QTextureImage();
//    texImage->setSource(QUrl::fromLocalFile("qml/tex.tif"));

//    uchar *prev = loadICSPreview();

    vector<VolumetricDataPtr> dataVec = VolumetricData::loadICS("K32_bassoon_TH_vGluT1_c01_cmle.ics");

    dataVec[0] = filterDemo(dataVec[0]);
//    dataVec[0]

    CustomDataTextureImage *texImage = new CustomDataTextureImage();
    texImage->setData(dataVec[0]->getConstData(), dataVec[0]->getWidth(), dataVec[0]->getHeight());
//    uchar *prev = loadICSPreview();
//    texImage->setData(prev, 512, 512);

    Qt3DRender::QAbstractTexture *tex = new Qt3DRender::QTexture2D();
    tex->addTextureImage(texImage);

    VolumeMaterial *mat = view.findChild<VolumeMaterial*>("objVol");
    mat->setTexture(tex);

    return app.exec();
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

PyObject* vdToList(VolumetricDataPtr vd)
{
    size_t n = vd->getWidth() * vd->getHeight();
    PyObject* listObj = PyList_New(n);
    if (!listObj) throw logic_error("Unable to allocate memory for Python list");
    for (unsigned int i = 0; i < n; i++) {
        PyObject *num = PyFloat_FromDouble((double) vd->getConstData()[i]);
        if (!num) {
            Py_DECREF(listObj);
            throw logic_error("Unable to allocate memory for Python list");
        }
        PyList_SET_ITEM(listObj, i, num);
    }
    return listObj;
}

void listToVd(PyObject* incoming, VolumetricDataPtr vd)
{
    if (PyList_Check(incoming)) {
        for (Py_ssize_t i = 0; i < PyList_Size(incoming); i++) {
            PyObject *value = PyList_GetItem(incoming, i);
            vd->getData()[i] = (uchar)PyFloat_AsDouble(value);
        }
    } else {
        cerr << "py return is not a list" << endl;
    }
}

VolumetricDataPtr filterDemo(VolumetricDataPtr vd)
{
    Py_SetProgramName((char*)"myPythonProgram");
    Py_Initialize();
    // parDict is a parameter to send to python function
    PyObject * parDict;
    parDict = PyDict_New();
    PyDict_SetItemString(parDict, "data", vdToList(vd));
    PyDict_SetItemString(parDict, "width", PyFloat_FromDouble(vd->getWidth()));
    PyDict_SetItemString(parDict, "height", PyFloat_FromDouble(vd->getHeight()));
    // run python code to load functions
    PyRun_SimpleString("exec(open('scripts/cpptest.py').read())");
    // get function showval from __main__
    PyObject* main_module = PyImport_AddModule("__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* func = PyDict_GetItemString(global_dict, "filter");
    // parameter should be a tuple
    PyObject* par = PyTuple_Pack(1, parDict);
    // call the function
    PyObject *pValue;
    pValue = PyObject_CallObject(func, par);
    cout << pValue << endl;
    listToVd(pValue, vd);
    Py_Finalize();

    return vd;
}

//uchar* loadICSPreview()
//{
//    uchar* data = nullptr;
//    size_t w, h;
//    IcsLoadPreview("K32_bassoon_TH_vGluT1_c01_cmle.ics", 18, (void**)&data, &w, &h);
//    cout << w << " " << h << endl;
//    return data;
//}

vector<VolumetricDataPtr> VolumetricData::loadICS(string filename)
{
    // TODO: use IcsGetOrder and prepare for shuffled dim order
    ICS* ip;
    IcsOpen(&ip, filename.c_str(), "r");

    Ics_DataType dt;
    int ndims;
    size_t dims[ICS_MAXDIM];
    IcsGetLayout(ip, &dt, &ndims, dims);
    size_t imageElementSize = IcsGetImelSize(ip);

    size_t w, h, d, c, n;
    c = ndims >= 4 ? dims[3] : 1;
    d = ndims >= 3 ? dims[2] : 1;
    h = ndims >= 2 ? dims[1] : 1;
    w = ndims >= 1 ? dims[0] : 1;
    n = w * h * d;

    ptrdiff_t strides[ndims];
    for (int j = 0; j < ndims; ++j) {
        strides[j] = 1;
        if (j == 3) {
            strides[j] = c;
        }
    }

    vector<VolumetricDataPtr> result;
    for (int k = 0; k < c; ++k) {
        VolumetricDataPtr vd = make_shared<VolumetricData>();
        vd->data = new uchar[w * h];
        vd->dims[0] = w;
        vd->dims[1] = h;
        vd->dims[2] = 1;
        IcsGetPreviewData(ip, vd->data, w * h, k * d);
        result.push_back(vd);
//        {
//            VolumetricDataPtr vd = make_shared<VolumetricData>();
//            vd->data = new uchar[n * imageElementSize];
//            vd->dims[0] = w;
//            vd->dims[1] = h;
//            vd->dims[2] = d;
//            // TODO: no offset was given to the data read
//            IcsGetDataWithStrides(ip, vd->data, 0, strides, ndims);
//            result.push_back(vd);
//        }
    }

    IcsClose(ip);
    return result;
}
