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
    logindialog.cpp \
    simplecrypt.cpp \
    QVERSO.pb.cc

HEADERS  += mainwindow.h \
    aboutdialog.h \
    confdialog.h \
    client.h \
    logindialog.h \
    simplecrypt.h \
    QVERSO.pb.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    confdialog.ui \
    logindialog.ui

DISTFILES += \
    QVERSO.proto

#Create .Desktop

isEmpty(PREFIX) {
PREFIX = /usr/local
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
CONFDIR = /etc

INSTALLS += target desktop icon32

## Instalar ejecutable
target.path = $$BINDIR

## Instalar acceso directo en el menú del escritorio
desktop.path = /usr/share/applications
desktop.files += qVersare.desktop

## Instalar icono de aplicación
icon32.path = $$DATADIR/icons/hicolor/32×32/apps
icon32.files += ./data/32×32/qversare-ico.png


PROTO = QVERSO.proto
include (protobuf.pri)
