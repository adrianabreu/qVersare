#include "qversaredatabase.h"
#include "utils.h"
#include <QSqlQuery>
#include <QString>
#include <QVariant>

QVersareDataBase::QVersareDataBase()
{

}

QVersareDataBase::QVersareDataBase(QSqlDatabase *ddbb,QCoreApplication *app,
                                   QString dbName,
                                   bool daemonMode)
{
    mydb_ = QSqlDatabase(*ddbb);
    mydb_.setDatabaseName("/var/lib/qVersareServer/" + dbName);

    if(!mydb_.open()) {
        helperDebug(daemonMode,
                    "Couldn't open ddbb, not possible to authenticate");
        app->exit(3); //Force exit
    }
    setupDatabase();
}

void QVersareDataBase::addMessage(QString room, QString username, QString message)
{
    QSqlQuery query(mydb_);
    query.prepare("INSERT INTO messages (room,username,message)"
                  "VALUES (:room, :username, :message)");
    query.bindValue(":room", room);
    query.bindValue(":username",username);
    query.bindValue(":message",message);
    query.exec();
}

QList<QVERSO> QVersareDataBase::getLastTenMessages(QString room)
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

QList<QVERSO> QVersareDataBase::getOthersUsersTimestamps(QList<QString> users)
{
    QList<QVERSO> aux;
    QVERSO auxVerso;
    //Hacer consulta
    QListIterator<QString> i(users);
    while(i.hasNext()) {
        QSqlQuery query(mydb_);
        query.prepare("SELECT AVTIMESTAMP FROM users"
                      " WHERE USERNAME=:username");
        query.bindValue(":username",i.next());
        query.exec();
        auxVerso.set_username(i.next().toStdString());
        auxVerso.set_timestamp(QDateTime::fromMSecsSinceEpoch(query.value(0)
                                                              .toInt())
                               .toString()
                               .toStdString());
        aux.push_back(auxVerso);
    }
    return aux;
}

QVERSO QVersareDataBase::getThisUserAvatar(QString user)
{
    QVERSO tempVerso;
    //Do the query, create a verso
    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM users WHERE "
                  "username=:username");
    query.bindValue(":username",user);
    query.exec();
    if(query.next()) {
        tempVerso.set_username(query.value("username").toString().toStdString());
        tempVerso.set_requestavatar(true);
        tempVerso.set_avatar(query.value("avatar").toString().toStdString());
        tempVerso.set_timestamp(QDateTime::fromMSecsSinceEpoch(
                                    query.value("avtimestamp").toInt())
                                .toString()
                                .toStdString());

    }
    return tempVerso;
}

QVERSO QVersareDataBase::getThisUserTimeStamp(QString user)
{
    QVERSO tempVerso;
    //Do the query, create a verso
    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM users WHERE "
                  "username=:username");
    query.bindValue(":username",user);
    query.exec();
    if(query.next()) {
        tempVerso.set_username(query.value("username").toString().toStdString());
        tempVerso.set_requestavatar(true);
        tempVerso.set_timestamp(QDateTime::fromMSecsSinceEpoch(
                                    query.value("avtimestamp").toInt())
                                .toString()
                                .toStdString());

    }
    return tempVerso;
}

void QVersareDataBase::setupDatabase()
{
    //Create table for users
    QSqlQuery query(mydb_);
    query.exec("CREATE TABLE IF NOT EXISTS users ("
                  "USERNAME VARCHAR(60) PRIMARY KEY,"
                  "PASSWORD VARCHAR(40),"
                  "AVATAR BLOB,"
                  "AVTIMESTAMP INTEGER)");
    //Create table for msgs
    query.exec("CREATE TABLE IF NOT EXISTS messages ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
               "ROOM VARCHAR(60),"
               "USERNAME VARCHAR(60),"
               "MESSAGE VARCHAR(2000),"
               "TIMESTAMP INTEGER)");

    //Insert basic users
    query.prepare("INSERT INTO users (username,password,avatar,avtimestamp)"
                  "VALUES (:username, :password,:avatar,:avtimestamp)");
    query.bindValue(":username","pepito");
    query.bindValue(":password","50648aff18d36a6b89cb7dcda2e4e8c5");
    query.bindValue(":avatar","null");
    query.bindValue(":avtimestamp",0);
    query.exec();

    query.prepare("INSERT INTO users (username,password,avatar,avtimestamp)"
                  "VALUES (:username, :password,:avatar,:avtimestamp)");
    query.bindValue(":username","tiger");
    query.bindValue(":password","9e95f6d797987b7da0fb293a760fe57e");
    query.bindValue(":avatar","null");
    query.bindValue(":avtimestamp",0);
    query.exec();

    query.prepare("INSERT INTO users (username,password,avatar,avtimestamp)"
                  "VALUES (:username, :password,:avatar,:avtimestamp)");
    query.bindValue(":username","qversare");
    query.bindValue(":password","3b867c3941a04ab062bba35d8a69a1d9");
    query.bindValue(":avatar","null");
    query.bindValue(":avtimestamp",0);

    query.exec();
}

void QVersareDataBase::updateClientAvatar(QString user,
                                        QString avatar,
                                        QDateTime timestamp)
{
    //Prepare query for update
    QSqlQuery query(mydb_);
    query.prepare("UPDATE users SET AVATAR = :avatar,"
                  "AVTIMESTAMP = :timestamp "
                  "WHERE USERNAME = :username");
    query.bindValue(":username", user);
    query.bindValue(":avatar", avatar);
    query.bindValue(":timestamp",timestamp.toMSecsSinceEpoch());
    if(!query.exec())
        qDebug() << query.lastError();
}

bool QVersareDataBase::goodCredentials(QString user, QString password)
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

QVersareDataBase::~QVersareDataBase()
{
    helperDebug(false,"Closing database");
    mydb_.close();
}
