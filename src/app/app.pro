QT += gui core widgets quick qml 3dcore 3drender 3dinput quickwidgets 3dextras
CONFIG += c++11
# for crashpad
CONFIG += force_debug_info
CONFIG += separate_debug_info

# used in version.h/cpp
DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=$$system(git describe --abbrev=8 --dirty --always --tags)

win32 {
    DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=win32
}

macx {
    DEFINES += DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=macx
}

macx {
    system(cp $${PWD}/lib/libics/libics_conf.h.in $${PWD}/lib/libics/libics_conf.h)
}

win32 {
    # TODO: copy libics_conf.in to libics_conf.h
}

HEADERS += \
    ../util/version.h \
    ../../lib/libics/libics_ll.h \
    ../../lib/libics/libics_intern.h \
    ../../lib/libics/libics.h \
    ../../lib/libics/libics_sensor.h

SOURCES +=  \
    ../util/version.cpp \
    main.cpp \
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

SOURCES += \
    ../core/directed_acyclic_graph/Node.cpp \
    ../core/directed_acyclic_graph/Graph.cpp \
    ../core/compute_platform/ports.cpp \
    ../core/compute_platform/ComputeModule.cpp \
    ../core/compute_platform/ComputePlatform.cpp \
    ../core/compute_platform/port_utils.hpp \
    ../core/multidim_image_platform/MultiDimImage.hpp \
    ../core/multidim_image_platform/MultiDimImage.cpp \
    ../core/high_platform/PythonComputeModule.cpp \
    ../core/io_utils/IcsAdapter.cpp

RESOURCES += resources.qrc

INCLUDEPATH += \
    ../util \
    ../core \
    ../../lib/libics \
    ../../lib/pybind11/include

INCLUDEPATH += \
    ../util \
    ../ \
    ../../lib/pybind11/include/ \
    ../../virtualenv/include/python3.6m

LIBS += -L"/Library/Frameworks/Python.framework/Versions/3.6/lib/" -lpython3.6m

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

    LIBS += -framework Cocoa -framework Security -lbsm

    crashpad.files = $$CRASHPAD_DIR/out/Release/crashpad_handler
    crashpad.path = Contents/MacOS

    QMAKE_BUNDLE_DATA += crashpad
}

win32 {
    CRASHPAD_DIR = $$_PRO_FILE_PWD_/../../../tools/crashpad/crashpad
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
    CRASHPAD_DST = $$shell_path($$clean_path($$OUT_PWD/crashpad_handler.exe))
    CopyCrashpad.commands = $$quote(cmd /c xcopy /Y /S /I $${CRASHPAD_SRC} $${CRASHPAD_DST})

    QMAKE_EXTRA_TARGETS += CopyCrashpad
    POST_TARGETDEPS += CopyCrashpad
}

LIBS += -lcrashpad_client -lbase -lcrashpad_handler_lib -lcrashpad_minidump -lcrashpad_util
