TARGET = test
SOURCES += \
    test.cpp \
    forward_network.cpp \
    ../core/forward_network/ForwardNetwork.cpp \
    ../core/forward_network/Node.cpp \
    ../core/forward_network/Group.cpp \
    compute_layer.cpp \
    ../core/compute_layer/Buffer.cpp \
    ../core/compute_layer/ComputeNode.cpp \

CONFIG -= app_bundle
INCLUDEPATH += \
    ../util \
    ../