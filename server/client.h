#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QThread>

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
    int socketFd_; //for remove from qmap
    QTcpSocket socket_;
    QPointer<QThread> thread_;
    bool logged_; //Store the status like a finite machine
};

#endif // CLIENT_H
