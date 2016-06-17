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
#include <QString>
#include <QTcpServer>
#include <QThread>

#include "client.h"
#include "qversaredatabase.h"
#include "qversarestatistics.h"
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

    //Stats Section
    void onTimeFromClient(QString type, int elapsedTime);

private:
    QMap<qintptr,QPointer<Client>> clients_;
    ServerSettings* settings_;
    QVersareDataBase mydb_;
    bool daemonMode_;
    QVersareStatistics mystats_;

    void addClientToList(QString room, Client *client);
    void removeClientFromList(QString room, Client *client);

    void sendThisAvatarToOthesUsers(QString user);

    QMap<QString, QList<Client*>> clientsPerRoom_;

};

#endif // QVERSARESERVER_H
