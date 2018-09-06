include($$TOP_SRC_DIR/common.pri)
include($$TOP_SRC_DIR/pybind11.pri)
include($$TOP_SRC_DIR/libics.pri)
include($$TOP_SRC_DIR/catch.pri)
include($$TOP_SRC_DIR/core.pri)

SOURCES += $$files(*.cpp, false)
HEADERS += $$files(*.h, false)
HEADERS += $$files(*.hpp, false)
HEADERS += $$files(*.ipp, false)

CONFIG -= app_bundle
CONFIG += console