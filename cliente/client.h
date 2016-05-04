#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>

class Client : public QObject
{
    Q_OBJECT
public:
    Client();
    Client(QString ip, int port, QString name);
    ~Client();

    //establecer conexion
    int connectTo();

    void makeConnect(QObject *parent);

    void sentTo(QString line);

    void log_me_in(QString username, QString password);

public slots:
    void recivedFrom();

signals:
    void messageRecive(QString message);

private:
    QTcpSocket socket_;
    QString ipServer_;
    int portServer_;
    QString userName_;
    bool connected_;
};

#endif // CLIENT_H
