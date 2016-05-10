#include <QByteArray>
#include <QDebug>
#include <QDataStream>

#include "client.h"
#include "qversareserver.h"
#include "QVERSO.pb.h"
#include "utils.h"

Client::Client(qintptr fd, bool daemonMode, QObject *parent) : QObject(parent),
    socket_(this),
    logged_(false)
{
    thread_ = new QThread(this);
    daemonMode_ = daemonMode;
    QString parseFd((quint32)fd);
    helperDebug(daemonMode_,parseFd);
    socket_.setSocketDescriptor(fd);
    socketFd_ = fd;
    room_ = "";


    makeConnections(parent);

    helperDebug(daemonMode_,"Client connected");
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
                 (buffer_size == 0) ) {
             in >> buffer_size;

         }
         if ( (buffer_size != 0) &&
                (socket_.bytesAvailable() >= buffer_size )) {
            algo=socket_.read(buffer_size);
            my_verso.ParseFromString(algo.toStdString());
            buffer_size = 0;

        } else {
             socket_.readAll();
        }

        if (!logged_) {
            if (my_verso.has_login() && my_verso.login() == true) {
                if (my_verso.has_username() && my_verso.has_password() ) {
                    emit validateMe(QString::fromStdString(my_verso.username() ),
                                  QString::fromStdString(my_verso.password()), this);
                }

            }
        } else {
            if (my_verso.room() != room_.toStdString()) {
                room_ = QString::fromStdString(my_verso.room());
                emit Client::imNewInTheRoom(room_, socketFd_);
            } else {
                emit forwardMessage(my_verso,socketFd_);
            }
        }
    }
}


void Client::newMessage(QVERSO a_verso, int fd)
{
    if (fd != socketFd_ &&
            a_verso.room() == room_.toStdString())
        sendVerso(a_verso);
}

void Client::deleteLater()
{
    helperDebug(daemonMode_,"Disconnected...");
    emit disconnectedClient(socketFd_);
}

void Client::readyValidate(bool status, Client *whoClient)
{
    if(whoClient == this) {
        logged_ = status;
        QVERSO logMessage;
        logMessage.set_login(status);
        sendVerso(logMessage);
        if(status == true) {
           room_ = "lobby";
           emit Client::imNewInTheRoom("lobby", socketFd_);
        }
    }
}

void Client::lastMessages(QVERSO a_verso, int fd)
{

    if(fd == socketFd_) {
        QString parseFd(fd);
        helperDebug(daemonMode_,
                    "I will send last 10 messages to this fd " + parseFd);
        sendVerso(a_verso);
    }
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

    connect(&socket_, &QTcpSocket::disconnected, static_cast<QThread*>(thread_),
            &QThread::quit);

    connect(this, &Client::imNewInTheRoom, static_cast<QVersareServer*>(parent),
            &QVersareServer::newInTheRoom);

    connect(static_cast<QVersareServer*>(parent),
            &QVersareServer::messageFromHistory,
            this, &Client::lastMessages);
}

void Client::sendVerso(QVERSO a_verso)
{
    //qDebug() << "Sending a verso";
    std::string buffer;
    a_verso.SerializeToString(&buffer);
    quint32 bufferSize = buffer.size();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)bufferSize;
    //out << buffer;

     if (socket_.write(block) == -1)
         helperDebug(daemonMode_,"Error!");

    if (socket_.write(buffer.c_str(), bufferSize) == - 1)
        helperDebug(daemonMode_,"Error writing the msg!");

    while(!socket_.waitForBytesWritten());
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
