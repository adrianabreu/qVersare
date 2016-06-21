#ifndef AVATARMANAGER_H
#define AVATARMANAGER_H

#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QList>
#include <QObject>
#include <QPair>
#include <QPixmap>
#include <QSize>
#include <QTextStream>
#include <QTimer>

#include "client.h"
#include "avatarmanager.h"


//Esta clase gestiona la lista de los avatares y los ficheros

class AvatarManager : public QObject
{
    Q_OBJECT
public:
    AvatarManager();
    ~AvatarManager();

    QPair<QPixmap,QSize> getDefaultAvatar();
    QPixmap getUserAvatar(QString username);

    QList<QPair<QString, QDateTime> > getLista();
    QString getPath() const;


    void updateAvatar(QString, QDateTime, QPixmap);
    int searchUser(QString username);
    int searchUser(QString username, QString ownUsername);

    void updateFile(void);

    void checkIfUpdateList(QString username);

    void refreshLocalUser(QString username, QDateTime time);

    void updateUserList(QString username, QDateTime time);
    void needAvatar(QString username, QDateTime time, Client *client);
    void addUser(QString username, QDateTime time);

    QString getUserAvatarFilePath(QString username);

public slots:
    void onTimerTimeout();

signals:
    void errorLoadingAvatar();

private:
    QFile *file_;
    QList<QPair<QString, QDateTime>> lista_;
    QString path_;
    QTimer myTimer_;

};

#endif // AVATARMANAGER_H
