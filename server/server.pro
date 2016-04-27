QT += core
QT -= gui
QT += network

CONFIG += c++11

TARGET = server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    qversareserver.cpp \
    client.cpp

HEADERS += \
    qversareserver.h \
    client.h
