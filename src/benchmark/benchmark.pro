CONFIG += c++14

QMAKE_CXXFLAGS += -fdiagnostics-absolute-paths

TARGET = benchmark
SOURCES += \
    benchmark.cpp \
    ../core/multidim_image_platform/MultiDimImage.hpp \
    ../core/multidim_image_platform/MultiDimImage.cpp \
    ../core/io_utils/IcsAdapter.cpp

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

HEADERS += \
    ../../lib/libics/libics_ll.h \
    ../../lib/libics/libics_intern.h \
    ../../lib/libics/libics.h \
    ../../lib/libics/libics_sensor.h

SOURCES +=  \
    ../../lib/libics/libics_preview.c \
    ../../lib/libics/libics_read.c \
    ../../lib/libics/libics_top.c \
    ../../lib/libics/libics_sensor.c \
    ../../lib/libics/libics_history.c \
    ../../lib/libics/libics_util.c \
    ../../lib/libics/libics_data.c \
    ../../lib/libics/libics_binary.c \
    ../../lib/libics/libics_test.c \
    ../../lib/libics/libics_write.c \
    ../../lib/libics/libics_compress.c