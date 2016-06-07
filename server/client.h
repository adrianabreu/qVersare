#ifndef CLIENT_H
#define CLIENT_H

#include <QDateTime>
#include <QObject>
#include <QPointer>
#include <QSslSocket>
#include <QThread>

#include "QVERSO.pb.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(qintptr fd, bool daemonMode, QString keyPath,
                    QString certPath, QObject *parent = 0);
    ~Client();

    void start();
    void die();

    bool getLogged() const;
    QString getName();

    //Connecting signal  & slots
    void makeConnections(QObject *parent);
    void makeMessageConnections(QObject *parent);
    void makeLoginConnections(QObject *parent);
    void makeAvatarConnections(QObject *parent);


    void sendVerso(QVERSO aVerso);
    void parseVerso(QVERSO aVerso);

    bool waitForEncryption();

signals:
    void forwardMessage(QVERSO aVerso, Client *fd);

    void disconnectedClient(int fd); //Sends the server with
                                     //object should be destroyed
    void imNewInTheRoom(QString room, Client *fd);

    void validateMe(QString user, QString password, Client *whoClient);

    void updateMyAvatar(QString user, QString avatar, QDateTime avtimestamp,
                        Client *whoClient);

    void requestThatAvatar(QString user, Client *fd);

    void deleteMeFromThisRoom(QString room, Client *fd);


public slots:
    void deleteLater();
    void readyRead();
    void readySslErrors(QList<QSslError> errors);

    //Messages
    //This is for forwarded messages
    void onMessageToOthers(QVERSO aVerso, Client *fd);
    //This is for answering request
    void onMessageToSame(QVERSO aVerso, Client *fd);

    //Login
    void readyValidate(bool status, Client *whoToValide);

    //Avatar section

private:
    int socketFd_; //for remove from qmap
    bool daemonMode_;
    QSslSocket socket_;
    QPointer<QThread> thread_;
    bool logged_; //Store the status like a finite machine
    QString room_; //Store the actual room of the client
    QString name_; //Store the name of the client
    void setupSecureMode(QString keyPath, QString certPath);
};

#endif // CLIENT_H
