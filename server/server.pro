QT += core
QT -= gui
QT += network
QT += sql

CONFIG += c++11

TARGET = server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    qversareserver.cpp \
    client.cpp \
    serversettings.cpp \
    utils.cpp \
    qversaredaemon.cpp

HEADERS += \
    qversareserver.h \
    client.h \
    serversettings.h \
    utils.h \
    qversaredaemon.h

DISTFILES += \
    users.db \
    QVERSO.proto

PROTOS = QVERSO.proto
include (protobuf.pri)
