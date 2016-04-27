#ifndef QVERSARESERVER_H
#define QVERSARESERVER_H

#include <QAbstractSocket>
#include <QDebug>
#include <QMap>
#include <QPointer>
#include <QObject>
#include <QTcpServer>
#include <QThread>
#include <QString>

#include "client.h"

class QVersareServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QVersareServer(QObject *parent = 0);
    ~QVersareServer();
    void startServer();

protected:
    /* Per new connection create a new thread
     * and a new socket */
    void incomingConnection(qintptr handle);

signals:
    void forwardedMessage(QString mymessage, int fd);

public slots:
    void newMessageFromClient(QString mymessage, int fd);
    void clientDisconnected(int fd);
    //void readyRead();

private:
    QMap<qintptr,QPointer<Client>> clients;


};

#endif // QVERSARESERVER_H
