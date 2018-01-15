TARGET = test
SOURCES += \
    test.cpp \
    directed_acyclic_graph_test.cpp \
    ../core/directed_acyclic_graph/Node.cpp \
    ../core/directed_acyclic_graph/Graph.cpp \
    compute_platform_test.cpp \

CONFIG -= app_bundle
CONFIG += debug
INCLUDEPATH += \
    ../util \
    ../