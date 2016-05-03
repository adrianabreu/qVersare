#include <QByteArray>
#include <QDebug>

#include "client.h"
#include "qversareserver.h"
#include "QVERSO.pb.h"

Client::Client(qintptr fd, QObject *parent) : QObject(parent),
    socket_(this),
    logged_(false)
{
    thread_ = new QThread(parent);
    qDebug() << fd;
    socket_.setSocketDescriptor(fd);
    socketFd_ = fd;
    //TO DO: rename attributes with _

    makeConnections(parent);

    qDebug() << "Client connected";
}

Client::~Client()
{
    socket_.close();

}


void Client::readyRead()
{
    QString messageToForward(socket_.readAll());

    if (!logged_) {
        QVERSO my_verso;
        my_verso.ParseFromString(messageToForward.toStdString() );
        if (my_verso.has_login() && my_verso.login() == true)
           if (my_verso.has_username() && my_verso.has_password() )
               emit validateMe(QString::fromStdString(my_verso.username()),
                               QString::fromStdString(my_verso.password()) );
    } else {
       emit forwardMessage(messageToForward, socket_.socketDescriptor());
    }

}

void Client::newMessage(QString message, int fd)
{
    if (fd != socket_.socketDescriptor())
        socket_.write(message.toUtf8());
}

void Client::deleteLater()
{
    qDebug() << "Disconnected...";
    emit disconnectedClient(socketFd_);
}

void Client::readyValidate(bool status)
{
    logged_ = status;
}

bool Client::getLogged() const
{
    return logged_;
}

void Client::makeConnections(QObject *parent)
{
    connect(&socket_, &QTcpSocket::readyRead, this, &Client::readyRead );

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

    connect(static_cast<QThread*>(thread_), &QThread::finished, this,
            &Client::deleteLater );

    connect(static_cast<QThread*>(thread_), &QThread::finished,
            static_cast<QThread*>(thread_), &QThread::deleteLater );

    connect(&socket_, &QTcpSocket::disconnected, static_cast<QThread*>(thread_),
            &QThread::quit);

}

void Client::start()
{
    this->moveToThread(thread_);
    thread_->start();
}

void Client::die()
{
    socket_.disconnectFromHost();
}
