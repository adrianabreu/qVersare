#include "client.h"
#include <QDebug>
#include <QByteArray>

Client::Client(qintptr fd, QObject *parent) : QObject(parent), my_socket(this)
{
    qDebug() << fd;
    my_socket.setSocketDescriptor(fd);
    connect(&my_socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(&my_socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(this,SIGNAL(forwardMessage(QString,int)),
            parent,SLOT(newMessageFromClient(QString,int)));
    connect(parent,SIGNAL(forwardedMessage(QString,int)),this,
            SLOT(newMessage(QString,int)));

    qDebug() << "Client connected";
}

void Client::disconnected()
{
    qDebug() << "Disconnected...";
}

void Client::readyRead()
{
    QString messageToForward(my_socket.readAll());
    emit forwardMessage(messageToForward, my_socket.socketDescriptor());
}

void Client::newMessage(QString message, int fd)
{
    if (fd != my_socket.socketDescriptor())
        my_socket.write(message.toUtf8());
}

void Client::start()
{
    this->setParent(0);
    this->moveToThread(&my_thread);
    my_thread.start();
}
