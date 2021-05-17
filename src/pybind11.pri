INCLUDEPATH *= $$TOP_SRC_DIR/../lib/pybind11/include

win* {
    INCLUDEPATH *= $$quote(C:\Python38-x64\include)
    LIBS *= -L$$quote(C:\Python38-x64\libs)
    LIBS *= $$quote(C:\Python38-x64\libs\python38.lib)
    QMAKE_CXXFLAGS *= -bigobj
}

macx {
    QMAKE_CXXFLAGS *= $$system(python3-config --includes)
    LIBS *= $$system(python3-config --ldflags)
}
