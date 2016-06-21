#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QThread>
#include <QDebug>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QPixmap>

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

    QString getName();

    void sendNewAvatar(QPixmap pixmap);

    void parseVerso(QVERSO my_verso);

    void setList(QList<QPair<QString, QDateTime>> lista);

    void askForAvatar(QString username);

public slots:
    void recivedFrom();

    void procesarErroresSsl(QList<QSslError> myList);

signals:
    void messageRecive(QString message);

    void avatar(QString username);

    void emitNeedAvatar(QString username, QDateTime time);

    void emitUpdateAvatar(QString username, QDateTime time, QPixmap image, bool same);

private:
    QSslSocket socket_;
    QString ipServer_;
    int portServer_;
    QString userName_;
    bool connected_;
    QString actualRoom_;
    QList<QPair<QString, QDateTime>> list_;
    qint32 largeChunkSize_;
    QByteArray buffer_;
};

#endif // CLIENT_H
