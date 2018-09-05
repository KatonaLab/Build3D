include($$TOP_SRC_DIR/common.pri)

SOURCES +=  \
    $$TOP_SRC_DIR/../lib/libics/libics_preview.c \
    $$TOP_SRC_DIR/../lib/libics/libics_read.c \
    $$TOP_SRC_DIR/../lib/libics/libics_top.c \
    $$TOP_SRC_DIR/../lib/libics/libics_sensor.c \
    $$TOP_SRC_DIR/../lib/libics/libics_history.c \
    $$TOP_SRC_DIR/../lib/libics/libics_util.c \
    $$TOP_SRC_DIR/../lib/libics/libics_data.c \
    $$TOP_SRC_DIR/../lib/libics/libics_binary.c \
    $$TOP_SRC_DIR/../lib/libics/libics_test.c \
    $$TOP_SRC_DIR/../lib/libics/libics_write.c \
    $$TOP_SRC_DIR/../lib/libics/libics_compress.c

HEADERS += \
    $$TOP_SRC_DIR/../lib/libics/libics_ll.h \
    $$TOP_SRC_DIR/../lib/libics/libics_intern.h \
    $$TOP_SRC_DIR/../lib/libics/libics.h \
    $$TOP_SRC_DIR/../lib/libics/libics_sensor.h

DEFINES += "LIBICS_USE_ZLIB=Off"
TEMPLATE = lib
CONFIG += staticlib
