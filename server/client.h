#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(qintptr fd, QObject *parent = 0);

    void start();
    void die();

signals:
    void forwardMessage(QString message, int fd);
    void disconnectedClient(int fd); //Sends the server with
                                     //object should be destroyed

public slots:
    void readyRead();
    void newMessage(QString message, int fd);
    void deleteLater();

private:
    int my_socket_fd; //for remove from qmap
    QTcpSocket my_socket;
    QThread my_thread;
};

#endif // CLIENT_H