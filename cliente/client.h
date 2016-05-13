#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QThread>
#include <QDebug>

#include <QVERSO.pb.h>

class Client : public QObject
{
    Q_OBJECT
public:
    Client();
    Client(QString ip, int port);
    ~Client();

    void sendUpdateRoom(QString room);
    //establecer conexion
    int connectTo();

    void makeConnect(QObject *parent);

    void sentTo(QVERSO a_verso);

    void log_me_in(QString username, QString password);

    void createMessageText(QString textLine);

    void setActualRoom(const QString &actualRoom);

    void setName(const QString &name);

public slots:
    void recivedFrom();

    void procesarErroresSsl(QList<QSslError> myList);

signals:
    void messageRecive(QString message);

private:
    QSslSocket socket_;
    QString ipServer_;
    int portServer_;
    QString userName_;
    bool connected_;
    QString actualRoom_;
};

#endif // CLIENT_H
