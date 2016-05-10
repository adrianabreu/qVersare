#include <QDebug>

#include <syslog.h>

#include "utils.h"

void helperDebug(bool daemon, QString arguments) {

    if (daemon)
        syslog(LOG_INFO,arguments.toStdString().c_str());
    else
        qDebug() << arguments;
}
