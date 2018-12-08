include($$TOP_SRC_DIR/common.pri)
include($$TOP_SRC_DIR/pybind11.pri)
include($$TOP_SRC_DIR/libics.pri)
include($$TOP_SRC_DIR/core.pri)
# include($$TOP_SRC_DIR/crashpad.pri)

SOURCES += $$files(*.cpp, false)
HEADERS += $$files(*.h, false)
HEADERS += $$files(*.hpp, false)
HEADERS += $$files(*.ipp, false)

QT += gui core widgets quick qml 3dcore 3drender 3dinput quickwidgets 3dextras concurrent

CONFIG += app_bundle
TARGET = a3-dc
RC_ICONS = ../../assets/icons/favicon.ico
RESOURCES += resources.qrc

win* {
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_DATE=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_DATE)
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA)
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_MODE)
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM)
}

macx {
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=$$system(git describe --dirty --always --tags)
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=macx

    release {
        DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=$$shell_quote(release)
    }
    debug {
        DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=$$shell_quote(debug)
    }

    BUILD_DATE = $$system(date "+%Y.%m.%d-%H.%M")
    DEFINES *= DEFINED_AT_COMPILATION_A3DC_BUILD_DATE=$$shell_quote($$BUILD_DATE)
}
