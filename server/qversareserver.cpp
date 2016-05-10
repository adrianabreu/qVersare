#include <QHostAddress>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "client.h"
#include "qversareserver.h"
#include "simplecrypt.h"
#include "utils.h"

QVersareServer::QVersareServer(QObject *parent, QCoreApplication *app,
                               ServerSettings *settings, QSqlDatabase * ddbb) :
    QTcpServer(parent)
{
    settings_ = settings;
    mydb_ = QSqlDatabase(*ddbb);
    mydb_.setDatabaseName(settings_->getDbName());
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
    helperDebug(false,"voy a cerrar la database");
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
    helperDebug(daemonMode_,"User: " + user);
    helperDebug(daemonMode_,"Password: " + password);

    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM users WHERE username=(:USERNAME)");
    query.bindValue(":USERNAME",user);
    query.exec();
    if (query.next()) {
        //Should change this for just a comparison
        SimpleCrypt crypto;
        crypto.setKey(0x02ad4a4acb9f023);
        QString stored_password(query.value("password").toString());
        QString str_pass = crypto.decryptToString(stored_password);
        QString rcv_pass = crypto.decryptToString(password);
        //qDebug() << str_pass;
        //qDebug() << rcv_pass;
        aux = (str_pass == rcv_pass);
    }
    //qDebug() << "Returning " << aux;
    return aux;
}

void QVersareServer::incomingConnection(qintptr handle)
{
    QPointer<Client> clientSocket = new Client(handle,daemonMode_, this);
    clients_.insert(clients_.end(),handle,clientSocket);
    //threads with parents are not movable
    clientSocket->setParent(0);
    clientSocket->start();

}

void QVersareServer::newMessageFromClient(QVERSO a_verso,int fd)
{
    //Good moment for store the message in the database
    QString room = QString::fromStdString(a_verso.room());
    QString username = QString::fromStdString(a_verso.username());
    QString message = QString::fromStdString(a_verso.message());

    addMessage(room,username, message);

    emit forwardedMessage(a_verso,fd);
}

void QVersareServer::clientDisconnected(int fd)
{
    QPointer<Client> temp = clients_.take(fd);
    delete temp;
}

void QVersareServer::validateClient(QString user, QString password, Client *whoClient)
{
    emit validateResult(goodCredentials(user,password),whoClient);
}

void QVersareServer::newInTheRoom(QString room, int fd)
{
    //Pense en simplemente emitir los mensajes y hacer los send_to
    //pero pasar toda esa lista de 10 mensajes cada vez... uff
    //mejor usar el map
    QList<QVERSO> lastMessages = lastTenMessages(room);
    QListIterator<QVERSO> it(lastMessages);
    //qDebug() << fd;
    QString parsingforDebug(lastMessages.size());
    helperDebug(daemonMode_,parsingforDebug);
    while(it.hasNext()) {
        emit messageFromHistory(it.next(),fd);
    }

}

void QVersareServer::setupDatabase()
{
    //Create table for users
    QSqlQuery query(mydb_);
    query.exec("CREATE TABLE IF NOT EXISTS users"
                  "USERNAME VARCHAR(60),"
                  "PASSWORD VARCHAR(60)"
                  "PRIMARY KEY (USERNAME)");
    //Add default user ?

    //Create table for msgs
    query.exec("CREATE TABLE IF NOT EXISTS messages ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
               "ROOM VARCHAR(60),"
               "USERNAME VARCHAR(60),"
               "MESSAGE VARCHAR(2000))");
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
