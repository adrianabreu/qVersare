#include <QVariant>
#include <QHostAddress>

#include "qversareserver.h"
#include "client.h"

QVersareServer::QVersareServer(QObject *parent) : QTcpServer(parent)
{

}

QVersareServer::~QVersareServer()
{
    for(QMap<qintptr,QPointer<Client>>::Iterator i = clients.begin();
        i != clients.end(); ++i) {
        i.value()->die();
    }
}

void QVersareServer::startServer()
{
    QSettings settings;

    if(!settings.contains("ip"))
        settings.setValue("ip", "127.0.0.1");
    if(!settings.contains("port"))
        settings.setValue("port",8000);

    QString ipAddress = settings.value("ip","127.0.0.1").toString();
    quint16 port = settings.value("port", 8000).toString().toUInt();

    if(!this->listen( QHostAddress(ipAddress), port) ){
        qDebug() << "Could not start server";
    } else {
        qDebug() << "Listening...";
    }
}

void QVersareServer::incomingConnection(qintptr handle)
{
    QPointer<Client> clientSocket = new Client(handle, this);
    clients.insert(clients.end(),handle,clientSocket);
    //threads with parents are not movable
    clientSocket->setParent(0);
    clientSocket->start();

}

void QVersareServer::newMessageFromClient(QString mymessage,int fd)
{
    qDebug() << mymessage;
    emit forwardedMessage(mymessage,fd);
}

void QVersareServer::clientDisconnected(int fd)
{
    QPointer<Client> temp = clients.take(fd);
    delete temp;
}
