#include <QDebug>
#include <QByteArray>

#include "client.h"
#include "qversareserver.h"
#include "QVERSO.pb.h"

Client::Client(qintptr fd, QObject *parent) : QObject(parent),
    my_socket(this),
    my_thread(this),
    logged_(false)
{
    qDebug() << fd;
    my_socket.setSocketDescriptor(fd);
    my_socket_fd = fd;
    //TO DO: create connection.... REALLY NEEDED
    //TO DO: rename attributes with _
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

    connect(this, &Client::validateMe,static_cast<QVersareServer*>(parent),
            &QVersareServer::validateClient);

    connect(static_cast<QVersareServer*>(parent),
            &QVersareServer::validateResult,this,&Client::readyValidate);

    connect(&my_thread, &QThread::finished, this, &Client::deleteLater );

    connect(&my_socket, &QTcpSocket::disconnected, &my_thread, &QThread::quit);

    qDebug() << "Client connected";
}


void Client::readyRead()
{
    QString messageToForward(my_socket.readAll());

    if (!logged_) {
        QVERSO my_verso;
        my_verso.ParseFromString(messageToForward.toStdString() );
        if (my_verso.has_login() && my_verso.login() == true)
           if (my_verso.has_username() && my_verso.has_password() )
               emit validateMe(QString::fromStdString(my_verso.username()),
                               QString::fromStdString(my_verso.password()) );
    } else {
       emit forwardMessage(messageToForward, my_socket.socketDescriptor());
    }

}

void Client::newMessage(QString message, int fd)
{
    if (fd != my_socket.socketDescriptor())
        my_socket.write(message.toUtf8());
}

void Client::deleteLater()
{
    qDebug() << "Disconnected...";
    emit disconnectedClient(my_socket_fd);
}

void Client::readyValidate(bool status)
{
    logged_ = status;
}

bool Client::getLogged() const
{
    return logged_;
}

void Client::start()
{
    this->moveToThread(&my_thread);
    my_thread.start();
}

void Client::die()
{
    my_socket.disconnectFromHost();
}
