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

BINDIR = /usr/sbin
DATADIR = /var/lib/$$TARGET
CONFDIR = /etc/$$TARGET
DAEMONDIR = /etc/init.d

INSTALLS += target data config daemonper

target.path = $$BINDIR

config.path = $$CONFDIR
config.files += $${TARGET}.pem $${TARGET}.conf
config.extra = mkdir -p $$CONFDIR #&& chown root:qversaredaemon $$CONFDIR/$${TARGET}.* && chmod 664 $$CONFDIR/$${TARGET}.*
config.commands = true

data.path = $$DATADIR
data.extra = mkdir -p $$DATADIR && if [ `getent passwd | grep -c '^qversaredaemon:'` -eq 0 ]; then groupadd qversaredaemon; useradd -r qversaredaemon -g qversaredaemon; fi && mkdir -p -m 775 $$DATADIR && chown root:qversaredaemon $$DATADIR && mkdir -p -m 775 $$DATADIR/avatares
data.commands = true

daemonper.path = $$DAEMONDIR
daemonper.extra += cp $${TARGET}.sh $$DAEMONDIR/$${TARGET}d && chmod +x $$DAEMONDIR/$${TARGET}d
daemonper.commands = true

