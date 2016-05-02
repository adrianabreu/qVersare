#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T12:46:39
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

TARGET = qVersare-Cli
CONFIG -= app_bundle
TEMPLATE = app
RC_FILE = qversare.rc


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    confdialog.cpp \
    client.cpp \
    logindialog.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    confdialog.h \
    client.h \
    logindialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    confdialog.ui \
    logindialog.ui

DISTFILES += \
    QVERSO.proto

PROTO = QVERSO.proto
include (protobuf.pri)
