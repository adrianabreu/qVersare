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
    explicit Client(qintptr fd, QObject *parent = 0);
    ~Client();

    void start();
    void die();

    bool getLogged() const;

    void makeConnections(QObject *parent);

    void sendVerso(QVERSO a_verso);

signals:
    void forwardMessage(QVERSO a_verso, int fd);
    void disconnectedClient(int fd); //Sends the server with
                                     //object should be destroyed
    void validateMe(QString user, QString password);

    void imNewInTheRoom(QString room, int fd);

public slots:
    void readyRead();
    void newMessage(QVERSO a_verso, int fd);
    void deleteLater();
    void readyValidate(bool status);
    //This is necessary because the fd must be the same
    void lastMessages(QVERSO a_verso, int fd);

private:
    int socketFd_; //for remove from qmap
    QTcpSocket socket_;
    QPointer<QThread> thread_;
    bool logged_; //Store the status like a finite machine
    QString room_; //Store the actual room of the client
};

#endif // CLIENT_H
