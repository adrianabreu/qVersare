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

    //establecer conexion
    int connectTo();
    void makeConnect(QObject *parent);

    //Getters
    QString getName();
    QString getLastUser() const;

    //Setters basicos
    void setActualRoom(const QString &actualRoom);
    void setBasicPath(QString path);
    void setList(QList<QPair<QString, QDateTime>> lista);
    void setName(const QString &name);

    //Funciones de envio
    void askForAvatar(QString username);
    void logMeIn(QString username, QString password);
    void sendNewAvatar(QPixmap pixmap);
    void sendTo(QVERSO aVerso);
    void sendUpdateRoom(QString room);

    //Funciones de recepcion
    void createMessageText(QString textLine, QDateTime timestamp);
    void parseVerso(QVERSO my_verso);
    void setLastUser(const QString &lastUser);
    void showMessage(QVERSO myVerso);


public slots:
    void recivedFrom();

    void procesarErroresSsl(QList<QSslError> myList);

signals:
    void messageReceived(QString message);

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
    QString basicPath_;
    QString lastUser_;
};


#endif // CLIENT_H
