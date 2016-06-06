#include <QDir>
#include <QHostAddress>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>


#include "client.h"
#include "qversareserver.h"
#include "utils.h"

QVersareServer::QVersareServer(QObject *parent, QCoreApplication *app,
                               ServerSettings *settings, QSqlDatabase * ddbb) :
    QTcpServer(parent)
{
    settings_ = settings;
    mydb_ = QSqlDatabase(*ddbb);
    mydb_.setDatabaseName("/var/lib/qVersareServer/" + settings_->getDbName());
    daemonMode_ = settings->getDaemon();
    if(!mydb_.open()) {
        helperDebug(daemonMode_,
                    "Couldn't open ddbb, not possible to authenticate");
        app->exit(3); //Force exit
    }
    setupDatabase();
    //Register metatype for queue QVERSOS in the msg loop
    qRegisterMetaType<QVERSO>("QVERSO");

}

QVersareServer::~QVersareServer()
{
    for(QMap<qintptr,QPointer<Client>>::Iterator i = clients_.begin();
        i != clients_.end(); ++i) {
        i.value()->die();
    }
    helperDebug(false,"Closing database");
    mydb_.close();
}

void QVersareServer::startServer()
{

    if(!this->listen( QHostAddress(settings_->getIpAddress()),
                      settings_->getPort()) )
        helperDebug(daemonMode_,"Could not start server");
    else
        helperDebug(daemonMode_,"Listening...");
}

bool QVersareServer::goodCredentials(QString user, QString password)
{
    //Check for db errors?
    bool aux = false;

    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM users WHERE username=(:USERNAME)");
    query.bindValue(":USERNAME",user);
    query.exec();
    if (query.next()) {
        //Compare md5
        QString stored_password(query.value("password").toString());
        aux = (password == stored_password);
    }
    //qDebug() << "Returning " << aux;
    return aux;
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

void QVersareServer::newMessageFromClient(QVERSO aVerso,int fd)
{
    //Good moment for store the message in the database
    QString room = QString::fromStdString(aVerso.room());
    QString username = QString::fromStdString(aVerso.username());
    QString message = QString::fromStdString(aVerso.message());

    addMessage(room,username, message);

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
    emit validateResult(goodCredentials(user,password),whoClient);
}

void QVersareServer::newInTheRoom(QString room, int fd)
{
    //Emitimos los 10 ultimos mensajess para el usuario conectado
    QList<QVERSO> lastMessages = lastTenMessages(room);
    QListIterator<QVERSO> it(lastMessages);
    while(it.hasNext()) {
        emit messageFromHistory(it.next(),fd);
    }

}

void QVersareServer::setupDatabase()
{
    //Create table for users
    QSqlQuery query(mydb_);
    query.exec("CREATE TABLE IF NOT EXISTS users ("
                  "USERNAME VARCHAR(60) PRIMARY KEY,"
                  "PASSWORD VARCHAR(40),"
                  "AVATAR BLOB,"
                  "AVTIMESTAMP VARCHAR(13))");
    //Create table for msgs
    query.exec("CREATE TABLE IF NOT EXISTS messages ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
               "ROOM VARCHAR(60),"
               "USERNAME VARCHAR(60),"
               "MESSAGE VARCHAR(2000),"
               "TIMESTAMP VARCHAR(13))");

    //Insert basic users
    query.prepare("INSERT ON CONFLICT IGNORE INTO users (username,password)"
                  "VALUES (:username, :password)");
    query.bindValue(":username","pepito");
    query.bindValue(":password","50648aff18d36a6b89cb7dcda2e4e8c5");
    query.exec();

    query.prepare("INSERT ON CONFLICT IGNORE INTO users (username,password)"
                  "VALUES (:username, :password)");
    query.bindValue(":username","tiger");
    query.bindValue(":password","9e95f6d797987b7da0fb293a760fe57e");
    query.exec();

    query.prepare("INSERT ON CONFLICT IGNORE INTO users (username,password)"
                  "VALUES (:username, :password)");
    query.bindValue(":username","qversare");
    query.bindValue(":password","3b867c3941a04ab062bba35d8a69a1d9");

    query.exec();
}

void QVersareServer::addMessage(QString room, QString username, QString message)
{
    QSqlQuery query(mydb_);
    query.prepare("INSERT INTO messages (room,username,message)"
                  "VALUES (:room, :username, :message)");
    query.bindValue(":room",room);
    query.bindValue(":username",username);
    query.bindValue(":message",message);
    query.exec();
}

QList<QVERSO> QVersareServer::lastTenMessages(QString room)
{
    QList<QVERSO>aux;
    //Query for extract the messages from the ddbb
    //The list comes on desc type!!
    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM messages WHERE room=(:ROOM) ORDER BY id "
                  "desc limit 10");
    query.bindValue(":ROOM",room);
    query.exec();

    while(query.next()) {
        QVERSO tempVerso;
        tempVerso.set_username(query.value("username").toString().toStdString());
        tempVerso.set_room(room.toStdString());
        tempVerso.set_message(query.value("message").toString().toStdString());
        aux.push_front(tempVerso);
    }

    return aux;

}
