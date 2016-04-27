#include "client.h"
#include <QDebug>
#include <QByteArray>
#include "qversareserver.h"

Client::Client(qintptr fd, QObject *parent) : QObject(parent),
    my_socket(this),
    my_thread(parent)
{
    qDebug() << fd;
    my_socket.setSocketDescriptor(fd);
    my_socket_fd = fd;

    connect(&my_socket, &QTcpSocket::disconnected, this, &Client::disconnected );
    connect(&my_socket, &QTcpSocket::readyRead, this, &Client::readyRead );

    connect(this, &Client::forwardMessage,
            static_cast<QVersareServer*>(parent),
            &QVersareServer::newMessageFromClient );

    connect(static_cast<QVersareServer*>(parent),
            &QVersareServer::forwardedMessage,
            this, &Client::newMessage);

    connect(this, &Client::disconnectedClient,
            static_cast<QVersareServer*>(parent),
            &QVersareServer::clientDisconnected);

    connect(&my_thread, &QThread::finished, this, &Client::deleteLater );

    connect(&my_socket, &QTcpSocket::disconnected, &my_thread, &QThread::quit);

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

void Client::deleteLater()
{
    emit disconnectedClient(my_socket_fd);
}

void Client::start()
{
    this->moveToThread(&my_thread);
    my_thread.start();
}
