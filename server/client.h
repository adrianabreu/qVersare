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

    bool getLogged() const;

signals:
    void forwardMessage(QString message, int fd);
    void disconnectedClient(int fd); //Sends the server with
                                     //object should be destroyed
    void validateMe(QString user, QString password);

public slots:
    void readyRead();
    void newMessage(QString message, int fd);
    void deleteLater();
    void readyValidate(bool status);

private:
    int my_socket_fd; //for remove from qmap
    QTcpSocket my_socket;
    QThread my_thread;
    bool logged_; //Store the status like a finite machine
};

#endif // CLIENT_H
