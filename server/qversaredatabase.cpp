#include "qversaredatabase.h"
#include "utils.h"
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QTimer>
QVersareDataBase::QVersareDataBase()
{

}

QVersareDataBase::QVersareDataBase(QSqlDatabase *ddbb,QCoreApplication *app,
                                   QString dbName,
                                   bool daemonMode)
{
    mydb_ = QSqlDatabase(*ddbb);
    mydb_.setDatabaseName("/var/lib/qVersareServer/" + dbName);
    daemonMode_ = daemonMode;
    if(!mydb_.open()) {
        helperDebug(daemonMode_,
                    "Couldn't open ddbb, not possible to authenticate");
        app->exit(3); //Force exit
    }
    setupDatabase();
}

void QVersareDataBase::addMessage(QString room, QString username,
                                  QString message, QString datetime)
{
    QSqlQuery query(mydb_);
    query.prepare("INSERT INTO messages (room,username,message,timestamp)"
                  "VALUES (:room, :username, :message, :timestamp)");
    query.bindValue(":room", room);
    query.bindValue(":username",username);
    query.bindValue(":message",message);
    query.bindValue(":timestamp",datetime);

    if (!query.exec())
        helperDebug(daemonMode_,"Error añadiendo el mensaje" +
                    query.lastError().text());
}

QList<QVERSO> QVersareDataBase::getLastTenMessages(QString room)
{
    QList<QVERSO>aux;
    //Query for extract the messages from the ddbb
    //The list comes on desc type!!
    QList<QString>users;

    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM messages WHERE room=(:ROOM) ORDER BY id "
                  "desc limit 10");
    query.bindValue(":ROOM",room);
    if (!query.exec()) {
        helperDebug(daemonMode_,query.lastError().text());
    } else {
        while(query.next()) {
            QVERSO tempVerso;
            if (!users.contains(query.value("username").toString())) {
                users.append(query.value("username").toString());
                QVERSO auxAvatar = getThisUserAvatar(query.value("username")
                                                     .toString());
                aux.push_front(auxAvatar);
            }
            tempVerso.set_username(query.value("username").toString()
                                   .toStdString());
            tempVerso.set_room(room.toStdString());
            tempVerso.set_message(query.value("message").toString()
                                  .toStdString());
            QDateTime timestamp;
            timestamp = QDateTime::fromString(query.value("timestamp")
                                              .toString(),
                                              "yyyy-MM-ddTHH:mm:ss");
            tempVerso.set_timestamp(timestamp
                                    .toString("yyyy-MM-ddTHH:mm:ss")
                                    .toStdString());
            aux.push_front(tempVerso);
        }
    }
    return aux;
}

QList<QVERSO> QVersareDataBase::getOthersUsersTimestamps(QList<QString> users)
{
    QList<QVERSO> aux;
    QVERSO auxVerso;
    //Hacer consulta
    QListIterator<QString> i(users);
    while (i.hasNext()) {
        QString auxName;
        auxName = i.next();
        QSqlQuery query(mydb_);
        query.prepare("SELECT AVTIMESTAMP FROM users"
                      " WHERE USERNAME = :username");
        query.bindValue(":username",auxName);
        if (!query.exec()) {
            helperDebug(daemonMode_,"Error getting other users tstamps" +
                        query.lastError().text());
        } else {
            if(query.next()) {
                auxVerso.set_username(auxName.toStdString());
                QDateTime auxTime = QDateTime::fromMSecsSinceEpoch(query.value(0)
                                                                .toInt());
                auxVerso.set_requestavatar(true);
                auxVerso.set_timestamp(auxTime.toString().toStdString());
                aux.push_back(auxVerso);
            }
        }

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
    if (!query.exec()) {
        helperDebug(daemonMode_, "No se ha podido hacer un select del tstamp" +
                    query.lastError().text());
    } else {
        if (query.next()) {
            tempVerso.set_username(query.value("username").toString()
                                   .toStdString());
            tempVerso.set_requestavatar(true);
            tempVerso.set_avatar(query.value("avatar").toString().toStdString());
            tempVerso.set_timestamp(query.value("avtimestamp").toString()
                                    .toStdString());
        }
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
    if (!query.exec()) {
        helperDebug(daemonMode_, "No se ha podido hacer un select del tstamp" +
                    query.lastError().text());
    } else {
        if(query.next()) {
            tempVerso.set_username(query.value("username").toString().toStdString());
            tempVerso.set_requestavatar(true);
            tempVerso.set_timestamp(query.value("avtimestamp").toString()
                                    .toStdString());
        } else {
            helperDebug(daemonMode_, "El usuario no tiene avatar registrado");
        }
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
                  "AVTIMESTAMP VARCHAR(20))");
    //Create table for msgs
    query.exec("CREATE TABLE IF NOT EXISTS messages ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
               "ROOM VARCHAR(60),"
               "USERNAME VARCHAR(60),"
               "MESSAGE VARCHAR(2000),"
               "TIMESTAMP VARCHAR(20))");

    //Insert basic users
    query.prepare("INSERT INTO users (username,password,avatar,avtimestamp)"
                  "VALUES (:username, :password,:avatar,:avtimestamp)");
    query.bindValue(":username","pepito");
    query.bindValue(":password","50648aff18d36a6b89cb7dcda2e4e8c5");
    query.bindValue(":avatar","");
    query.bindValue(":avtimestamp","1970-01-01T00:00:00");
    query.exec();

    query.prepare("INSERT INTO users (username,password,avatar,avtimestamp)"
                  "VALUES (:username, :password,:avatar,:avtimestamp)");
    query.bindValue(":username","tiger");
    query.bindValue(":password","9e95f6d797987b7da0fb293a760fe57e");
    query.bindValue(":avatar","");
    query.bindValue(":avtimestamp","1970-01-01T00:00:00");
    query.exec();

    query.prepare("INSERT INTO users (username,password,avatar,avtimestamp)"
                  "VALUES (:username, :password,:avatar,:avtimestamp)");
    query.bindValue(":username","qversare");
    query.bindValue(":password","3b867c3941a04ab062bba35d8a69a1d9");
    query.bindValue(":avatar","");
    query.bindValue(":avtimestamp","1970-01-01T00:00:00");

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
    query.bindValue(":timestamp",timestamp.toString("yyyy-MM-ddTHH:mm:ss"));
    if(!query.exec())
        helperDebug(daemonMode_, "Error updating avatar ");
}

bool QVersareDataBase::goodCredentials(QString user, QString password)
{
    bool aux = false;
    QSqlQuery query(mydb_);
    query.prepare("SELECT * FROM users WHERE username=(:USERNAME)");
    query.bindValue(":USERNAME",user);
    if (!query.exec()) {
        helperDebug(daemonMode_, "Error selecting avatar " +
                    query.lastError().text());
    } else {
        if (query.next()) {
            //Compare md5
            QString stored_password(query.value("password").toString());
            aux = (password == stored_password);
        }
    }
    return aux;
}

QVersareDataBase::~QVersareDataBase()
{
    helperDebug(false,"Closing database");
    mydb_.close();
}
