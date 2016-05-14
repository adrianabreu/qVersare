QT += core
QT -= gui
QT += network
QT += sql

CONFIG += c++11

TARGET = qVersareServer
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

BINDIR = /usr/local
DATADIR = /var/lib/$$TARGET
CONFDIR = /etc/$$TARGET
DAEMONDIR = /etc/init.d

INSTALLS += target config data daemonper

target.path = $$BINDIR

config.path = $$CONFDIR
config.files += $${TARGET}.pem
config.extra = chmod 666 $$CONFDIR/$${TARGET}.pem
config.commands = true

data.path = $$DATADIR
data.extra = useradd qversaredaemon && mkdir -p -m 775 $$DATADIR && chown root:qversaredaemon $$DATADIR
data.commands = true

daemonper.path = $$DAEMONDIR
daemonper.extra += cp $${TARGET}.sh $$DAEMONDIR/$${TARGET} && chmod +x $$DAEMONDIR/$${TARGET}d.sh
daemonper.commands = true

