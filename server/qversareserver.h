#ifndef QVERSARESERVER_H
#define QVERSARESERVER_H

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDebug>
#include <QMap>
#include <QObject>
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
    explicit QVersareServer(QObject *parent = 0, QCoreApplication* app = 0);
    ~QVersareServer();
    void startServer();
    bool goodCredentials(QString user, QString password);

protected:
    /* Per new connection create a new thread
     * and a new socket */
    void incomingConnection(qintptr handle);

signals:
    void forwardedMessage(QVERSO a_verso, int fd);
    void validateResult(bool status);

public slots:
    void newMessageFromClient(QVERSO a_verso, int fd);
    void clientDisconnected(int fd);
    void validateClient(QString user, QString password);

private:
    QMap<qintptr,QPointer<Client>> clients_;
    ServerSettings* settings;
    QSqlDatabase mydb_;

    //Create db tables and basic structure
    void setupDatabase();
    //Add a message to history table
    void addMessage(QString room, QString username, QString message);

};

#endif // QVERSARESERVER_H
