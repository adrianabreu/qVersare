#!/bin/sh
#
# qVersareServer    /etc/init.d initscript para qVersare
#

### BEGIN INIT INFO
# Provides:          qVersareServer
# Required-Start:    $network $local_fs $remote_fs
# Required-Stop:     $network $local_fs $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: inicia y detiene qVersareServer
# Description:       qVersare daemon permite ejectutar el servidor 
#                    de qVersare de forma "invisible"
### END INIT INFO

# Salir inmediatamente si un comando falla
# http://www.gnu.org/software/bash/manual/bashref.html#The-Set-Builtin
set -e

# Importar funciones LSB:
# start_daemon, killproc, status_of_proc, log_*, etc.
. /lib/lsb/init-functions

NAME=qVersareServer
PIDFILE=/var/run/$NAME.pid
DAEMON=/usr/sbin/$NAME
DAEMON_OPTS="--daemon"

# Si el demonio no existe, salir.
test -x $DAEMON || exit 5

start()
{
    log_daemon_msg "Starting the $NAME process"
    start_daemon -p $PIDFILE -- $DAEMON $DAEMON_OPTS
    log_end_msg $?
}

stop()
{
    log_daemon_msg "Stoppping the $NAME process"
    killproc -p $PIDFILE
    log_end_msg $?
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    status)
        if [ -e $PIDFILE ]; then
            status_of_proc -p $PIDFILE $DAEMON "$NAME process"
        else
            log_failure_msg "$NAME process is not running"
            log_end_msg 0
        fi
        ;;
    restart)
        stop
        start
        ;;
    reload|force-reload)
        if [ -e $PIDFILE ]; then
            killproc -p $PIDFILE -SIGHUP
            log_success_msg "$NAME process reloaded successfully"
        else
            log_failure_msg "$NAME process is not running"
            log_end_msg 0
        fi
        ;;
    *)
        echo "Usage:  $0 {start|stop|status|reload|force-reload|restart}"
        exit 2
        ;;
esac
