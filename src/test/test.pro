TARGET = test
SOURCES += \
    test.cpp \
    forward_network.cpp \
    ../core/forward_network/ForwardNetwork.cpp \
    ../core/forward_network/Node.cpp \
    ../core/forward_network/Group.cpp

CONFIG -= app_bundle
INCLUDEPATH += \
    ../util \
    ../