LIBS += -lcrashpad_client -lbase -lcrashpad_handler_lib -lcrashpad_minidump -lcrashpad_util

macx {
    # TODO: highly environment dependent
    CRASHPAD_DIR = $$PWD/../../tools/crashpad/crashpad
    INCLUDEPATH += $$CRASHPAD_DIR
    INCLUDEPATH += $$CRASHPAD_DIR/third_party/mini_chromium/mini_chromium/
    debug {
        LIBS += -L"$$CRASHPAD_DIR/out/Debug"
    }
    release {
        LIBS += -L"$$CRASHPAD_DIR/out/Release"
    }

    LIBS += -framework Cocoa -framework Security -lbsm -lz

    crashpad.files = $$CRASHPAD_DIR/out/Release/crashpad_handler
    crashpad.path = Contents/MacOS

    QMAKE_BUNDLE_DATA += crashpad
}

win* {
    isEmpty(CRASHPAD_DIR) {
        CRASHPAD_DIR = $$PWD/../../tools/crashpad/crashpad
    }
    INCLUDEPATH += $$CRASHPAD_DIR
    INCLUDEPATH += $$CRASHPAD_DIR/third_party/mini_chromium/mini_chromium/
    INCLUDEPATH += $$CRASHPAD_DIR/third_party/zlib/zlib/
    debug {
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
    release {
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
