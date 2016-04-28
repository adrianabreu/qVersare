#include <QVariant>
#include <QHostAddress>

#include "qversareserver.h"
#include "client.h"

QVersareServer::QVersareServer(QObject *parent, QCoreApplication *app) :
    QTcpServer(parent)
{
    settings = new ServerSettings(app);
}

QVersareServer::~QVersareServer()
{
    for(QMap<qintptr,QPointer<Client>>::Iterator i = clients_.begin();
        i != clients_.end(); ++i) {
        i.value()->die();
    }
}

void QVersareServer::startServer()
{

    if(!this->listen( QHostAddress(settings->getIpAddress()),
                      settings->getPort()) )
        qDebug() << "Could not start server";
    else
        qDebug() << "Listening...";
}

void QVersareServer::incomingConnection(qintptr handle)
{
    QPointer<Client> clientSocket = new Client(handle, this);
    clients_.insert(clients_.end(),handle,clientSocket);
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
    QPointer<Client> temp = clients_.take(fd);
    delete temp;
}
