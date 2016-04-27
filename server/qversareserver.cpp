#include "qversareserver.h"
#include "client.h"

QVersareServer::QVersareServer(QObject *parent) : QTcpServer(parent)
{

}

void QVersareServer::startServer()
{
    if(!this->listen(QHostAddress::Any,8000)) {
        qDebug() << "Could not start server";
    } else {
        qDebug() << "Listening...";
    }
}

void QVersareServer::incomingConnection(qintptr handle)
{
    //QThread *newClient = new QThread;
    QPointer<Client> clientSocket = new Client(handle, this);
    clients.insert(clients.end(),handle,clientSocket);
    //threads with parents are not movable
    clientSocket->start();

}

void QVersareServer::newMessageFromClient(QString mymessage,int fd)
{
    qDebug() << mymessage;
    emit forwardedMessage(mymessage,fd);
}

void QVersareServer::clientDisconnected(int fd)
{
    clients.remove(fd);

}
