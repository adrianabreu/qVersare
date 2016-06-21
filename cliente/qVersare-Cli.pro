#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T12:46:39
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia
QT       += multimediawidgets

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
    QVERSO.pb.cc \
    loaddialog.cpp \
    cameradialog.cpp \
    avatarmanager.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    confdialog.h \
    client.h \
    logindialog.h \
    QVERSO.pb.h \
    loaddialog.h \
    cameradialog.h \
    avatarmanager.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    confdialog.ui \
    logindialog.ui \
    loaddialog.ui \
    cameradialog.ui

DISTFILES += \
    QVERSO.proto

#Create .Desktop

isEmpty(PREFIX) {
PREFIX = /usr/local
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
CONFDIR = /etc

INSTALLS += target desktop icon32 avatar

## Instalar ejecutable
target.path = $$BINDIR

avatar.path = .
avatar.extra = mkdir -p ~/.local/share/qVersare && cp qVersareDefaultAvatar.jpg ~/.local/share/qVersare/. && chown $$(USER) ~/.local/share/qVersare && chown $$(USER) ~/.local/share/qVersare/qVersareDefaultAvatar.jpg
avatar.commands = true
#avatar.files += qVersareDefaultAvatar.jpg

## Instalar acceso directo en el menú del escritorio
desktop.path = /usr/share/applications
desktop.files += qVersare.desktop~/.local/share/qVersare

## Instalar icono de aplicación
icon32.path = /usr/share/icons/hicolor/32x32/apps
icon32.files += ./data/32x32/qversare-ico.png
icon32.files += ./data/32x32/qversare-ico.xpm

PROTO = QVERSO.proto
include (protobuf.pri)
