include($$TOP_SRC_DIR/common.pri)
include($$TOP_SRC_DIR/pybind11.pri)
include($$TOP_SRC_DIR/libics.pri)

SOURCES += $$files(*.cpp, true)
HEADERS += $$files(*.h, true)
HEADERS += $$files(*.hpp, true)
HEADERS += $$files(*.ipp, true)

INCLUDEPATH += $$PWD/..

TEMPLATE = lib
CONFIG += staticlib