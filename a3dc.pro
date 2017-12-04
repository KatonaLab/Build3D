QT += gui core widgets quick qml 3dcore 3drender 3dinput quickwidgets 3dextras
CONFIG += c++14

macx {
    system(cp $${PWD}/lib/libics/libics_conf.h.in $${PWD}/lib/libics/libics_conf.h)
}

win32 {
    # TODO: copy libics_conf.in to libics_conf.h
}

HEADERS += \
    src/volumetric.h \
    lib/libics/libics_ll.h \
    lib/libics/libics_intern.h \
    lib/libics/libics.h \
    lib/libics/libics_sensor.h

SOURCES +=  \
    src/volumetric.cpp \
    src/main.cpp \
    lib/libics/libics_preview.c \
    lib/libics/libics_read.c \
    lib/libics/libics_top.c \
    lib/libics/libics_sensor.c \
    lib/libics/libics_history.c \
    lib/libics/libics_util.c \
    lib/libics/libics_data.c \
    lib/libics/libics_binary.c \
    lib/libics/libics_test.c \
    lib/libics/libics_write.c \
    lib/libics/libics_gzip.c \
    lib/libics/libics_compress.c

RESOURCES += resources.qrc

INCLUDEPATH += \
    lib/libics \
    lib/pybind11/include

macx {
    INCLUDEPATH += $$_PRO_FILE_PWD_/../tools/crashpad/crashpad
    INCLUDEPATH += $$_PRO_FILE_PWD_/../tools/crashpad/crashpad/third_party/mini_chromium/mini_chromium/
    CONFIG(debug, debug|release) {
        LIBS += -L"$$_PRO_FILE_PWD_/../tools/crashpad/crashpad/out/Debug"
    }
    CONFIG(release, debug|release) {
        LIBS += -L"$$_PRO_FILE_PWD_/../tools/crashpad/crashpad/out/Release"
    }
}

win32 {
    INCLUDEPATH += $$_PRO_FILE_PWD_/../tools/crashpad/crashpad
    INCLUDEPATH += $$_PRO_FILE_PWD_/../tools/crashpad/crashpad/third_party/mini_chromium/mini_chromium/
    CONFIG(debug, debug|release) {
        LIBS += -L"$$_PRO_FILE_PWD_/../tools/crashpad/crashpad/out/Debug"
    }
    CONFIG(release, debug|release) {
        LIBS += -L"$$_PRO_FILE_PWD_/../tools/crashpad/crashpad/out/Release"
    }
}

LIBS += -lcrashpad_client -lbase -lcrashpad_handler_lib -lcrashpad_minidump -lcrashpad_util

macx {
    LIBS += -framework Cocoa -framework Security -lbsm
}