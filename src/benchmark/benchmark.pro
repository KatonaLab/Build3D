include(../common.pri)
include(../pybind11.pri)
include(../libics.pri)
include(../core.pri)
include(../benchpress.pri)

SOURCES += $$files(*.cpp, false)
HEADERS += $$files(*.h, false)
HEADERS += $$files(*.hpp, false)
HEADERS += $$files(*.ipp, false)

CONFIG -= app_bundle
CONFIG += console