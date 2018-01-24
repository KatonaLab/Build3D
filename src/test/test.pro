TARGET = test
SOURCES += \
    test.cpp \
    directed_acyclic_graph_test.cpp \
    ../core/directed_acyclic_graph/Node.cpp \
    ../core/directed_acyclic_graph/Graph.cpp \
    compute_platform_test.cpp \
    compute_platform_test_helper.hpp \
    ../core/compute_platform/ports.cpp \
    ../core/compute_platform/ComputeModule.cpp \
    ../core/compute_platform/ComputePlatform.cpp \
    ../core/compute_platform/port_utils.hpp \
    multidim_image_platform_test.cpp \
    ../core/multidim_image_platform/MultiDimImage.hpp \
    ../core/multidim_image_platform/MultiDimImage.cpp

CONFIG -= app_bundle
CONFIG += debug
INCLUDEPATH += \
    ../util \
    ../