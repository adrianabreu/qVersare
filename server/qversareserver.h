#ifndef QVERSARESERVER_H
#define QVERSARESERVER_H

#include <QAbstractSocket>
#include <QDebug>
#include <QList>
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
    void startServer();

protected:
    /* Per new connection create a new thread
     * and an indepent socket
     */
    void incomingConnection(qintptr handle);

signals:
    void forwardedMessage(QString mymessage, int fd);

public slots:
    void newMessageFromClient(QString mymessage, int fd);
    //void readyRead();

private:
    QList<QPointer<Client>> clients;


};

#endif // QVERSARESERVER_H
