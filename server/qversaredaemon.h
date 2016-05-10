#ifndef QVERSAREDAEMON_H
#define QVERSAREDAEMON_H

#include <QSocketNotifier>
#include <QObject>
#include <QCoreApplication>
#include <QPointer>

#include "qversareserver.h"
#include "serversettings.h"

class QVersareDaemon : public QObject
{
    Q_OBJECT
public:
    explicit QVersareDaemon(ServerSettings *settings, QCoreApplication *a,
                            QObject *parent = 0);
    ~QVersareDaemon();

    static void termSignalHandler(int unused);
    static void hupSignalHandler(int unused);

    bool setupUnixSignalHandlers();

signals:

public slots:
    void handleSigTerm();
    void handleSigHup();

private:
    static int sigTermSd[2];
    static int sigHupSd[2];

    QSocketNotifier *sigTermNotifier;
    QSocketNotifier *sigHupNotifier;

    QPointer<QVersareServer> server_;

    ServerSettings *settings_;
    QCoreApplication *app_;
    QSqlDatabase database_;
};

#endif // QVERSAREDAEMON_H
