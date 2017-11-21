QT += gui core widgets quick qml 3dcore 3drender 3dinput
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/volumetric.h \
    lib/libics/libics_conf.h \
    lib/libics/libics_ll.h \
    lib/libics/libics_intern.h \
    lib/libics/libics.h \
    lib/libics/libics_sensor.h

INCLUDEPATH += \
    lib/libics

#LIBS += -L build_ics -llibics_static