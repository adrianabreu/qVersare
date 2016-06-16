#include <QDateTime>
#include <QDir>
#include <QHostAddress>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>


#include "client.h"
#include "qversareserver.h"
#include "utils.h"

QVersareServer::QVersareServer(QObject *parent, QCoreApplication *app,
                               ServerSettings *settings, QSqlDatabase *ddbb) :
    QTcpServer(parent),mydb_(ddbb,app,settings->getDbName(),settings->getDaemon())
{
    settings_ = settings;
    //Register metatype for queue QVERSOS in the msg loop
    qRegisterMetaType<QVERSO>("QVERSO");
    daemonMode_ = settings->getDaemon();

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

    if(!this->listen( QHostAddress(settings_->getIpAddress()),
                      settings_->getPort()) )
        helperDebug(daemonMode_,"Could not start server");
    else
        helperDebug(daemonMode_,"Listening...");
}


void QVersareServer::incomingConnection(qintptr handle)
{
    //Comprobar que no se han eliminado los ficheros de clave y cert
    QDir filesDir;
    QString rutaPem = "/etc/qVersareServer/qVersareServer.pem";
    if(filesDir.exists(rutaPem)) {
        QPointer<Client> clientSocket = new Client(handle,daemonMode_,
                                                   rutaPem,
                                                   rutaPem,
                                                   this);
        clients_.insert(clients_.end(),handle,clientSocket);
        //threads with parents are not movable

        if (!clientSocket->waitForEncryption()){
            helperDebug(daemonMode_, "No se pudo hacer el handhsake");
            clientSocket->die();
        } else {
            clientSocket->makeConnections(this);
            clientSocket->setParent(0);
            clientSocket->start();
        }
    } else {
        helperDebug(daemonMode_, "Verificar ficheros .key y .crt");
    }

}

void QVersareServer::newMessageFromClient(QVERSO aVerso,Client *fd)
{
    //Good moment for store the message in the database
    QString room = QString::fromStdString(aVerso.room());
    QString username = QString::fromStdString(aVerso.username());
    QString message = QString::fromStdString(aVerso.message());

    mydb_.addMessage(room,username, message);

    emit forwardedMessage(aVerso,fd);
}

void QVersareServer::clientDisconnected(int fd)
{
    QPointer<Client> temp = clients_.take(fd);
    delete temp;
}

void QVersareServer::validateClient(QString user, QString password,
                                    Client *whoClient)
{
    emit validateResult(mydb_.goodCredentials(user,password),whoClient);
}

void QVersareServer::newInTheRoom(QString room, Client *fd)
{
    //Emitimos los 10 ultimos mensajess para el usuario conectado
    QList<QVERSO> lastMessages = mydb_.getLastTenMessages(room);
    QListIterator<QVERSO> it(lastMessages);
    while(it.hasNext()) {
        emit messageFromHistory(it.next(),fd);
    }

    //Emitimos los timestamps de los avatares de los demás al usuario
    QListIterator<Client*> i(clientsPerRoom_.value(room));
    QList<QString> clientsNames;
    while(i.hasNext())
        clientsNames.append(i.next()->getName());
    QList<QVERSO> otherUsersTimeStamp = mydb_.getOthersUsersTimestamps(clientsNames);


    QListIterator<QVERSO> it2(otherUsersTimeStamp);
    while(it2.hasNext()) {
        emit userTimeStamp(it.next(), fd);
    }
    //Añadimos al usuario a la lista
    helperDebug(daemonMode_, "User added to room: " + room);
    addClientToList(room, fd);
}

void QVersareServer::removeMeFromRoom(QString room, Client *fd)
{
    removeClientFromList(room, fd);
}

void QVersareServer::updateClientAvatar(QString user, QString avatar,
                                        QDateTime timestamp)
{
    mydb_.updateClientAvatar(user, avatar, timestamp);
}



void QVersareServer::onRequestedAvatar(QString user, Client *fd)
{
    emit userAvatar(mydb_.getThisUserAvatar(user), fd);
}

void QVersareServer::onRequestedTimestamp(QString user, Client *fd)
{
    emit userTimeStamp(mydb_.getThisUserTimeStamp(user), fd);
}

void QVersareServer::addClientToList(QString room, Client *client)
{
    QList<Client*> aux;
    if(clientsPerRoom_.contains(room)) {
        aux = clientsPerRoom_.take(room);
        aux.append(client);
        clientsPerRoom_.insert(room, aux);
    } else {
        aux.append(client);
        clientsPerRoom_.insert(room, aux);
    }
}

void QVersareServer::removeClientFromList(QString room, Client *client)
{
    QList<Client*> aux;
    if(clientsPerRoom_.contains(room)) {
        aux = clientsPerRoom_.take(room);
        if(aux.contains(client))
            aux.removeOne(client);
    }
    if(!aux.empty())
        clientsPerRoom_.insert(room, aux);
}
