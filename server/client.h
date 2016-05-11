#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QThread>

#include "QVERSO.pb.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(qintptr fd, bool daemonMode, QObject *parent = 0);
    ~Client();

    void start();
    void die();

    bool getLogged() const;

    void makeConnections(QObject *parent);

    void sendVerso(QVERSO aVerso);

signals:
    void forwardMessage(QVERSO aVerso, int fd);

    void disconnectedClient(int fd); //Sends the server with
                                     //object should be destroyed
    void imNewInTheRoom(QString room, int fd);

    void validateMe(QString user, QString password, Client *whoClient);


public slots:
    void deleteLater();
    //This is necessary because the fd must be the same
    void lastMessages(QVERSO aVerso, int fd);
    //This is for forwarded messages
    void newMessage(QVERSO aVerso, int fd);

    void readyRead();
    void readyValidate(bool status, Client* whoToValide);

private:
    int socketFd_; //for remove from qmap
    bool daemonMode_;
    QTcpSocket socket_;
    QPointer<QThread> thread_;
    bool logged_; //Store the status like a finite machine
    QString room_; //Store the actual room of the client

};

#endif // CLIENT_H
