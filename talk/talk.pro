TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread

SOURCES += main.cpp \
    socket.cpp \
    tcpserver.cpp \
    client.cpp \
    server.cpp \
    helpsignalsthreads.cpp \
    history.cpp

HEADERS += \
    socket.h \
    tcpserver.h \
    client.h \
    server.h \
    helpsignalsthreads.h \
    history.h
