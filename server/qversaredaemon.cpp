#include <signal.h> //sigaction
#include <sys/socket.h>
#include <unistd.h>

#include "serversettings.h"
#include "qversaredaemon.h"
#include "utils.h"

//Static definition
int QVersareDaemon::sigTermSd[2];
int QVersareDaemon::sigHupSd[2];

QVersareDaemon::QVersareDaemon(ServerSettings *settings,
                               QCoreApplication *a,
                               QObject *parent) : QObject(parent)
{

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigTermSd))
        qFatal("Couldn't create TERM socketpair");

    sigTermNotifier = new QSocketNotifier(sigTermSd[1],
            QSocketNotifier::Read, this);

    connect(sigTermNotifier,&QSocketNotifier::activated, this,
            &QVersareDaemon::handleSigTerm);

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigHupSd))
        qFatal("Couldn't create TERM socketpair");

    sigHupNotifier = new QSocketNotifier(sigHupSd[1],
            QSocketNotifier::Read, this);

    connect(sigHupNotifier,&QSocketNotifier::activated, this,
            &QVersareDaemon::handleSigHup);

    app_ = a;
    settings_ = settings;
    database_ = QSqlDatabase::addDatabase("QSQLITE");
    server_ = new QVersareServer(this, app_,settings_, &database_);
    server_->startServer();

}

QVersareDaemon::~QVersareDaemon()
{

}

void QVersareDaemon::termSignalHandler(int unused)
{
    char a = 1;
    write(sigTermSd[0], &a, sizeof(a));
}

void QVersareDaemon::hupSignalHandler(int unused)
{
    char a = 1;
    write(sigHupSd[0], &a, sizeof(a));
}

void QVersareDaemon::handleSigTerm()
{
    sigTermNotifier->setEnabled(false);

    char tmp;
    read(sigTermSd[1], &tmp, sizeof(tmp));

    //Do stuff
    helperDebug(true,"Voy a morir por mi propia causa");
    delete server_;
    qApp->quit();

    sigTermNotifier->setEnabled(true);

}

void QVersareDaemon::handleSigHup()
{
    sigHupNotifier->setEnabled(false);

    char tmp;
    read(sigHupSd[1], &tmp, sizeof(tmp));

    helperDebug(true,"Received sighup, reloading");
    //Cerramos conexiones y cargamos la nueva conf
    delete settings_;
    delete server_;

    settings_ = new ServerSettings(app_);

    server_ = new QVersareServer(this, app_, settings_, &database_);
    server_->startServer();

    sigHupNotifier->setEnabled(true);
}

bool QVersareDaemon::setupUnixSignalHandlers()
{
    struct::sigaction term;

    term.sa_handler = &QVersareDaemon::termSignalHandler;

    sigemptyset(&term.sa_mask);

    term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
        return false;

    struct::sigaction hup;

    hup.sa_handler = &QVersareDaemon::hupSignalHandler;

    sigemptyset(&hup.sa_mask);

    hup.sa_flags = SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0) > 0)
        return false;

    return true;
}
