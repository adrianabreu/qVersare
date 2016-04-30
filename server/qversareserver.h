#ifndef QVERSARESERVER_H
#define QVERSARESERVER_H

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QSettings>
#include <QSqlDatabase>
#include <QString>
#include <QTcpServer>
#include <QThread>


#include "client.h"
#include "serversettings.h"

class QVersareServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QVersareServer(QObject *parent = 0, QCoreApplication* app = 0);
    ~QVersareServer();
    void startServer();
    bool goodCredentials(QString user, QString password);

protected:
    /* Per new connection create a new thread
     * and a new socket */
    void incomingConnection(qintptr handle);

signals:
    void forwardedMessage(QString mymessage, int fd);
    void validateResult(bool status);

public slots:
    void newMessageFromClient(QString mymessage, int fd);
    void clientDisconnected(int fd);
    void validateClient(QString user, QString password);
    //void readyRead();

private:
    QMap<qintptr,QPointer<Client>> clients_;
    ServerSettings* settings;
    QSqlDatabase mydb_;

};

#endif // QVERSARESERVER_H
