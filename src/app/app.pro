QT += gui core widgets quick qml 3dcore 3drender 3dinput quickwidgets 3dextras
CONFIG += c++14
#CONFIG += force_debug_info # for crashpad
#CONFIG += separate_debug_info # for crashpad
CONFIG += no_keywords # whihtout this config compiler complains about PyType_Slot *slots; /* terminated by slot==0. */ in Python.h (since slots is a restricted keyword in Qt)

# CONFIG -= app_bundle

TARGET = a3-dc
RC_ICONS = ../../assets/icons/favicon.ico

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}

win32 {
    QMAKE_CXXFLAGS += -bigobj
}

message($$quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA))

DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_DATE=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_DATE)
DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA)
DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_MODE)
DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=$$shell_quote($$DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM)

macx {
    # used in version.h/cpp
    DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=$$system(git describe --dirty --always --tags)
    DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=macx

    CONFIG(release, debug|release) {
        DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=$$shell_quote(release)
    }

    CONFIG(debug, debug|release) {
        DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=$$shell_quote(debug)
    }

    BUILD_DATE = $$system(date "+%Y.%m.%d-%H.%M")
    DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_DATE=$$shell_quote($$BUILD_DATE)

    QMAKE_CXXFLAGS += -fdiagnostics-absolute-paths
    QMAKE_CXXFLAGS += -Wshadow
    # QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
}

SOURCES += $$files("*.cpp", true)
SOURCES += $$files("../core/*.cpp", true)
SOURCES += $$files("../util/*.cpp", true)
HEADERS += $$files("../*.h", true)
HEADERS += $$files("../*.hpp", true)
HEADERS += $$files("../*.ipp", true)

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
    ../../lib/libics/libics_gzip.c \
    ../../lib/libics/libics_compress.c

# HEADERS += \
#     ../../lib/libics/libics_ll.h \
#     ../../lib/libics/libics_intern.h \
#     ../../lib/libics/libics.h \
#     ../../lib/libics/libics_sensor.h

# //HEADERS += \
# //    VolumeData.h \
# //    VolumeTexture.h \
# //    LogCollector.h \
# //    TurnTableCameraController.h \
# //    BackendStore.h \
# //    BackendStoreItem.h \
# //    BackendInput.h \
# //    BackendParameter.h \
# //    BackendOutput.h \
# //    BackendModule.h \
# //    IcsDataSourceModule.h \
# //    ParameterInterfaceModules.hpp \
# //    OutputInterfaceModules.hpp \
# //    GlobalSettings.h \
# //    ../util/version.h \
# //    ../../lib/libics/libics_ll.h \
# //    ../../lib/libics/libics_intern.h \
# //    ../../lib/libics/libics.h \
# //    ../../lib/libics/libics_sensor.h
# //
# //SOURCES += \
# //    ../util/version.cpp \
# //    VolumeData.cpp \
# //    VolumeTexture.cpp \
# //    LogCollector.cpp \
# //    TurnTableCameraController.cpp \
# //    BackendStore.cpp \
# //    BackendStoreItem.cpp \
# //    BackendInput.cpp \
# //    BackendParameter.cpp \
# //    BackendOutput.cpp \
# //    BackendModule.cpp \
# //    IcsDataSourceModule.cpp \
# //    ParameterInterfaceModules.cpp \
# //    OutputInterfaceModules.cpp \
# //    main.cpp \
# //    GlobalSettings.cpp
# //
# //SOURCES +=  \
# //    ../../lib/libics/libics_preview.c \
# //    ../../lib/libics/libics_read.c \
# //    ../../lib/libics/libics_top.c \
# //    ../../lib/libics/libics_sensor.c \
# //    ../../lib/libics/libics_history.c \
# //    ../../lib/libics/libics_util.c \
# //    ../../lib/libics/libics_data.c \
# //    ../../lib/libics/libics_binary.c \
# //    ../../lib/libics/libics_test.c \
# //    ../../lib/libics/libics_write.c \
# //    ../../lib/libics/libics_gzip.c \
# //    ../../lib/libics/libics_compress.c
# //
# //SOURCES += \
# //    ../core/directed_acyclic_graph/Node.cpp \
# //    ../core/directed_acyclic_graph/Graph.cpp \
# //    ../core/compute_platform/ports.cpp \
# //    ../core/compute_platform/ComputeModule.cpp \
# //    ../core/compute_platform/ComputePlatform.cpp \
# //    ../core/compute_platform/ModuleContext.cpp \
# //    ../core/compute_platform/port_utils.hpp \
# //    ../core/multidim_image_platform/MultiDimImage.hpp \
# //    ../core/multidim_image_platform/MultiDimImage.cpp \
# //    ../core/high_platform/PythonComputeModule.cpp \
# //    ../core/io_utils/IcsAdapter.cpp

RESOURCES += resources.qrc

INCLUDEPATH += \
    ../ \
    ../util \
    ../core \
    ../../lib/libics \
    ../../lib/pybind11/include

INCLUDEPATH += \
    ../util \
    ../ \
    ../../lib/pybind11/include/

win32 {
    INCLUDEPATH += "C:\WinPython36\python-3.6.5.amd64/include/"
    LIBS += "C:\WinPython36\python-3.6.5.amd64/libs/libpython36.a"
    LIBS += -L"C:\WinPython36\python-3.6.5.amd64/libs/"
    DEFINES += "LIBICS_USE_ZLIB=Off" # for libics
    DEFINES += "DO_NOT_USE_WMAIN" # for catch.hpp
    SOURCES -= ../../lib/libics/libics_gzip.c
}

macx {
    #INCLUDEPATH += \
    #    ../../virtualenv/include/python3.6m
    #LIBS += -L"/usr/local/Cellar/python/3.6.5/Frameworks/Frameworks/Python.framework/Versions/3.6/lib" -lpython3.6m
    QMAKE_CXXFLAGS += $$system(python3-config --includes)
    LIBS += $$system(python3-config --ldflags)
}

macx {
    CRASHPAD_DIR = $$_PRO_FILE_PWD_/../../../tools/crashpad/crashpad
    INCLUDEPATH += $$CRASHPAD_DIR
    INCLUDEPATH += $$CRASHPAD_DIR/third_party/mini_chromium/mini_chromium/
    CONFIG(debug, debug|release) {
        LIBS += -L"$$CRASHPAD_DIR/out/Debug"
    }
    CONFIG(release, debug|release) {
        LIBS += -L"$$CRASHPAD_DIR/out/Release"
    }

    LIBS += -framework Cocoa -framework Security -lbsm -lz

    crashpad.files = $$CRASHPAD_DIR/out/Release/crashpad_handler
    crashpad.path = Contents/MacOS

    QMAKE_BUNDLE_DATA += crashpad
}

win32 {
    isEmpty(CRASHPAD_DIR) {
        CRASHPAD_DIR = $$_PRO_FILE_PWD_/../../../tools/crashpad/crashpad
    }
    INCLUDEPATH += $$CRASHPAD_DIR
    INCLUDEPATH += $$CRASHPAD_DIR/third_party/mini_chromium/mini_chromium/
    INCLUDEPATH += $$CRASHPAD_DIR/third_party/zlib/zlib/
    CONFIG(debug, debug|release) {
        LIBS += -L"$$CRASHPAD_DIR/out/Debug"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/client"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/compat"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/handler"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/minidump"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/third_party/mini_chromium/mini_chromium/base"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/third_party/zlib"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/tools"
        LIBS += -L"$$CRASHPAD_DIR/out/Debug/obj/util"
    }
    CONFIG(release, debug|release) {
        LIBS += -L"$$CRASHPAD_DIR/out/Release"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/client"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/compat"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/handler"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/minidump"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/third_party/mini_chromium/mini_chromium/base"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/third_party/zlib"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/tools"
        LIBS += -L"$$CRASHPAD_DIR/out/Release/obj/util"
    }

    LIBS += -ladvapi32

    CRASHPAD_SRC = $$shell_path($$clean_path("$$CRASHPAD_DIR/out/Release/crashpad_handler.exe"))
    CONFIG(debug, debug|release) {
        CRASHPAD_DST = $$shell_path($$clean_path($$OUT_PWD/debug))
    }
    CONFIG(release, debug|release) {
        CRASHPAD_DST = $$shell_path($$clean_path($$OUT_PWD/release))
    }
    CopyCrashpad.commands = $$QMAKE_COPY $$shell_quote($${CRASHPAD_SRC}) $$shell_quote($${CRASHPAD_DST})

    QMAKE_EXTRA_TARGETS += CopyCrashpad
    POST_TARGETDEPS += CopyCrashpad
}

LIBS += -lcrashpad_client -lbase -lcrashpad_handler_lib -lcrashpad_minidump -lcrashpad_util
