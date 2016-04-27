#include "client.h"
#include <QDebug>
#include <QByteArray>

Client::Client(qintptr fd, QObject *parent) : QObject(parent), my_socket(this)
{
    qDebug() << fd;
    my_socket.setSocketDescriptor(fd);
    my_socket_fd = fd;
    connect(&my_socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(&my_socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(this,SIGNAL(forwardMessage(QString,int)),
            parent,SLOT(newMessageFromClient(QString,int)));
    connect(parent,SIGNAL(forwardedMessage(QString,int)),this,
            SLOT(newMessage(QString,int)));
    connect(this,SIGNAL(disconnectedClient(int)),parent,SLOT(clientDisconnected(int)));

    qDebug() << "Client connected";
}

Client::~Client()
{
    qDebug() << "Called destructor";
    my_thread.exit();
}

void Client::disconnected()
{
    qDebug() << "Disconnected...";
    emit disconnectedClient(my_socket_fd);
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
