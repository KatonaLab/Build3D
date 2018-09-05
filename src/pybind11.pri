INCLUDEPATH *= $$TOP_SRC_DIR/../lib/pybind11/include

win* {
    # TODO: env specific, make it independent
    INCLUDEPATH *= $$quote(C:\WinPython36\python-3.6.5.amd64\include)
    LIBS *= -L$$quote(C:\WinPython36\python-3.6.5.amd64\libs)
    LIBS *= $$quote(C:\WinPython36\python-3.6.5.amd64\libs\libpython36.a)
    QMAKE_CXXFLAGS *= -bigobj
}

macx {
    QMAKE_CXXFLAGS *= $$system(python3-config --includes)
    LIBS *= $$system(python3-config --ldflags)
}