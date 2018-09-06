INCLUDEPATH *= $$TOP_SRC_DIR/../lib/libics

win*:debug {
	LIBS *= -L"$$TOP_BUILD_DIR/lib/libics/debug" -llibics
}

win*:release {
	LIBS *= -L"$$TOP_BUILD_DIR/lib/libics/release" -llibics
}

macx {
	LIBS *= "$$TOP_BUILD_DIR/lib/libics/liblibics.a"
}
