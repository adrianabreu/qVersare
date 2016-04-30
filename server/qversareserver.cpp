#include <QHostAddress>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "client.h"
#include "qversareserver.h"

QVersareServer::QVersareServer(QObject *parent, QCoreApplication *app) :
    QTcpServer(parent)
{
    settings = new ServerSettings(app);
    mydb_ = QSqlDatabase::addDatabase("QSQLITE");
    mydb_.setDatabaseName(settings->getDbName());
    if(!mydb_.open()) {
        qDebug() << "Couldn't open ddbb, not possible authenticate";
    }
}

QVersareServer::~QVersareServer()
{
    for(QMap<qintptr,QPointer<Client>>::Iterator i = clients_.begin();
        i != clients_.end(); ++i) {
        i.value()->die();
    }
    mydb_.close();
}

void QVersareServer::startServer()
{

    if(!this->listen( QHostAddress(settings->getIpAddress()),
                      settings->getPort()) )
        qDebug() << "Could not start server";
    else
        qDebug() << "Listening...";
}

bool QVersareServer::goodCredentials(QString user, QString password)
{
    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM users WHERE username=(:USERNAME) AND password=(:PASSWORD)");
    query.bindValue(":USERNAME",user);
    query.bindValue(":PASSWORD",password);
    query.exec();
    if (query.next())
        qDebug() << "LOGGED";
    else
        qDebug()<< "NOT LOGGED";
    return false;
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

void QVersareServer::validateClient(QString user, QString password)
{
    emit validateResult(goodCredentials(user,password));
}
