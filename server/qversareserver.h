#ifndef QVERSARESERVER_H
#define QVERSARESERVER_H

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPointer>
#include <QSettings>
#include <QSqlDatabase>
#include <QString>
#include <QTcpServer>
#include <QThread>

#include "client.h"
#include "QVERSO.pb.h"
#include "serversettings.h"


class QVersareServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QVersareServer(QObject *parent = 0, QCoreApplication *app = 0,
                            ServerSettings *settings = 0, QSqlDatabase *ddbb = 0);
    ~QVersareServer();
    void startServer();
    bool goodCredentials(QString user, QString password);

protected:
    /* Per new connection create a new thread
     * and a new socket */
    void incomingConnection(qintptr handle);

signals:
    void forwardedMessage(QVERSO aVerso, Client *fd);
    void validateResult(bool status, Client *whoClient);
    void messageFromHistory(QVERSO aVerso, Client *fd);

    //Avatar section
    void userTimeStamp(QVERSO aVerso, Client *fd);

    void userAvatar(QVERSO aVerso, Client *fd);

public slots:
    void newMessageFromClient(QVERSO aVerso, Client *fd);
    void clientDisconnected(int fd);

    void validateClient(QString user, QString password, Client *whoClient);

    void newInTheRoom(QString room, Client *fd);

    void removeMeFromRoom(QString room, Client *fd);

    //Avatar Section
    void updateClientAvatar(QString user, QString avatar, QDateTime timestamp);

    void onRequestedAvatar(QString user, Client *fd);

    void onRequestedTimestamp(QString user, Client *fd);


private:
    QMap<qintptr,QPointer<Client>> clients_;
    ServerSettings* settings_;
    QSqlDatabase mydb_;
    bool daemonMode_;
    //Create db tables and basic structure
    void setupDatabase();
    //Add a message to history table
    void addMessage(QString room, QString username, QString message);
    QList<QVERSO> getLastTenMessages(QString room);

    //Avatar section
    QList<QVERSO> getOthersUsersTimestamps(QString room);
    QVERSO getThisUserAvatar(QString user);
    QVERSO getThisUserTimeStamp(QString user);

    void addClientToList(QString room, Client *client);
    void removeClientFromList(QString room, Client *client);

    void sendThisAvatarToOthesUsers(QString user);

    QMap<QString, QList<Client*>> clientsPerRoom_;

};

#endif // QVERSARESERVER_H
