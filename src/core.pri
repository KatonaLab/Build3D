INCLUDEPATH *= $$PWD

win32*:debug {
	LIBS *= -L"$$TOP_BUILD_DIR/core/debug" -lcore
}

win32*:release {
	LIBS *= -L"$$TOP_BUILD_DIR/core/release" -lcore
}

macx {
	LIBS *= "$$TOP_BUILD_DIR/core/libcore.a"
}
