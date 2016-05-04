#include <QByteArray>
#include <QDebug>
#include <QDataStream>

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
    qint32 buffer_size = 0;
    QVERSO my_verso;
    while(socket_.bytesAvailable() > 0){
        QByteArray algo;
        QDataStream in(&socket_);

         if (socket_.bytesAvailable() >= (int)( sizeof(qint32) ) &&
                 (buffer_size == 0) )
         {
             in >> buffer_size;

         } if ( (buffer_size != 0) && (socket_.bytesAvailable() >= buffer_size )) {
            algo=socket_.read(buffer_size);
            my_verso.ParseFromString(algo.toStdString());
            buffer_size = 0;

        } else {
           socket_.readAll();
        }
    }

    if (!logged_) {
        if (my_verso.has_login() && my_verso.login() == true)
           if (my_verso.has_username() && my_verso.has_password() ) {
               emit validateMe(QString::fromStdString(my_verso.username()),
                               QString::fromStdString(my_verso.password()) );
           }
    } else {
        emit forwardMessage(my_verso, socket_.socketDescriptor());
    }

}

void Client::newMessage(QVERSO a_verso, int fd)
{
    if (fd != socket_.socketDescriptor())
        sendVerso(a_verso);
}

void Client::deleteLater()
{
    qDebug() << "Disconnected...";
    emit disconnectedClient(socketFd_);
}

void Client::readyValidate(bool status)
{
    logged_ = status;
    QVERSO logMessage;
    logMessage.set_login(status);
    sendVerso(logMessage);
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

void Client::sendVerso(QVERSO a_verso)
{
    std::string buffer;
    a_verso.SerializeToString(&buffer);
    quint32 bufferSize = buffer.size();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)bufferSize;

    socket_.write(block);

    socket_.write(buffer.c_str(), bufferSize);
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
