#ifndef QVERSAREDATABASE_H
#define QVERSAREDATABASE_H

#include <QCoreApplication>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QSqlError>
#include <QDebug>
#include "QVERSO.pb.h"

class QVersareDataBase
{
public:
    QVersareDataBase();

    QVersareDataBase(QSqlDatabase *ddbb, QCoreApplication *app,
                     QString dbName, bool daemonMode);

    ~QVersareDataBase();

    //Create db tables and basic structure
    void setupDatabase();
    //Add a message to history table
    void addMessage(QString room, QString username, QString message);
    QList<QVERSO> getLastTenMessages(QString room);

    //Avatar section
    QList<QVERSO> getOthersUsersTimestamps(QList<QString> usernames);
    QVERSO getThisUserAvatar(QString user);
    QVERSO getThisUserTimeStamp(QString user);
    bool goodCredentials(QString user, QString password);
    void updateClientAvatar(QString user,QString avatar, QDateTime timestamp);
private:
    QSqlDatabase mydb_;
    bool daemonMode_;

};

#endif // QVERSAREDATABASE_H
