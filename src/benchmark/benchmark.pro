CONFIG += c++14

QMAKE_CXXFLAGS += -fdiagnostics-absolute-paths

TARGET = test
SOURCES += \
    benchmark.cpp \
    ../core/multidim_image_platform/MultiDimImage.hpp \
    ../core/multidim_image_platform/MultiDimImage.cpp \

CONFIG -= app_bundle
CONFIG += debug
INCLUDEPATH += \
    ../util \
    ../ \
    ../../lib/pybind11/include/ \
    ../../lib/libics

win32 {
    INCLUDEPATH += "C:\WinPython36\python-3.6.5.amd64/include/"
    LIBS += "C:\WinPython36\python-3.6.5.amd64/libs/libpython36.a"
    LIBS += -L"C:\WinPython36\python-3.6.5.amd64/libs/"
    QMAKE_CXXFLAGS += -bigobj
    SOURCES -= ../../lib/libics/libics_gzip.c
    DEFINES += "LIBICS_USE_ZLIB=Off" # for libics
    DEFINES += "DO_NOT_USE_WMAIN" # for catch.hpp
    CONFIG += console
}

macx {
    # INCLUDEPATH += \
        # ../../virtualenv/include/python3.6m
    # LIBS += -L"/Library/Frameworks/Python.framework/Versions/3.6/lib" -lpython3.6m
    QMAKE_CXXFLAGS += $$system(python3-config --includes)
    LIBS += $$system(python3-config --ldflags)
}
