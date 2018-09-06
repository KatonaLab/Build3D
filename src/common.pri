# Note: using some nice advices from
# https://tobias-men.de/qmake-steroids-cutting-build-times-half/

CONFIG *= c++14
# Without 'no_keywords' config, compiler complains about 
# 'PyType_Slot *slots; /* terminated by slot==0. */ in 
# Python.h' (since 'slots' is a restricted keyword in Qt)
# for projects using Qt and pybind11 at the same time.
# Note that this config should only appear in Python
# involving .pro or .pri files, but as it can require
# to alter your Qt code (e.g.: using 'Q_SIGNALS' instead
# of 'signals') we wanted to make this config option
# appear upfront.
CONFIG *= no_keywords

# for crashpad
CONFIG += force_debug_info

CONFIG += warn_on

OBJECTS_DIR = objects
MOC_DIR = mocs

macx {
    QMAKE_CXXFLAGS *= -fdiagnostics-absolute-paths
    QMAKE_CXXFLAGS *= -Wshadow
    QMAKE_CXXFLAGS *= -g -fsanitize=address -fno-omit-frame-pointer
    QMAKE_LFLAGS *= -fsanitize=address
}

win* {
    QMAKE_CXXFLAGS *= -bigobj
}

release {
    CONFIG *= optimize_full
}

INCLUDEPATH *= $$PWD/util