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
    serversettings.cpp

HEADERS += \
    qversareserver.h \
    client.h \
    serversettings.h

DISTFILES += \
    users.db
