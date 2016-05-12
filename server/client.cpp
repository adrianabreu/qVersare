#include <QByteArray>
#include <QDebug>
#include <QDataStream>
#include <QSslCertificate>
#include <QSsl>
#include <QFile>
#include <QSslKey>
#include <QTextStream>
#include <QList>
#include "client.h"
#include "qversareserver.h"
#include "QVERSO.pb.h"
#include "utils.h"

Client::Client(qintptr fd, bool daemonMode, QString keyPath,
               QString certPath, QObject *parent) : QObject(parent),
    socket_(this),
    logged_(false)
{
    thread_ = new QThread(this);
    daemonMode_ = daemonMode;
    socketFd_ = fd;
    room_ = "";

    socket_.setPrivateKey(keyPath);
    QFile myCert("./" + certPath);

    if(myCert.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QSslCertificate serverCert(myCert.readAll());
        QList<QSslCertificate> caCert;
        caCert.push_back(serverCert);
        socket_.setCaCertificates(caCert);
        myCert.close();
        QSslError error(QSslError::SelfSignedCertificate, caCert.at(0));
        QList<QSslError>  expectedSslErrors;
        expectedSslErrors.append(error);
        expectedSslErrors.append(QSslError::CertificateUntrusted);

        socket_.ignoreSslErrors(expectedSslErrors);
        socket_.setLocalCertificate(serverCert);

        socket_.setSocketDescriptor(fd);
        connect(&socket_,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(errorPaso(QAbstractSocket::SocketError)));
        socket_.startServerEncryption();
        helperDebug(daemonMode_,"Client connected");

    } else {
        helperDebug(daemonMode_, "Fallo al abrir el cert");

    }
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
            if (my_verso.login()) {
                emit validateMe(QString::fromStdString(my_verso.username() ),
                             QString::fromStdString(my_verso.password()), this);
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


void Client::newMessage(QVERSO aVerso, int fd)
{
    if (fd != socketFd_ && aVerso.room() == room_.toStdString())
        sendVerso(aVerso);
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

void Client::lastMessages(QVERSO aVerso, int fd)
{
    //While to the forwarding we just want to send to the anothers fd
    //here we sant to send to the same client
    if(fd == socketFd_)
        sendVerso(aVerso);
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

void Client::sendVerso(QVERSO aVerso)
{
    //qDebug() << "Sending a verso";
    std::string buffer;
    aVerso.SerializeToString(&buffer);
    quint32 bufferSize = buffer.size();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)bufferSize;

     if (socket_.write(block) == -1)
         helperDebug(daemonMode_,"Error!");

    if (socket_.write(buffer.c_str(), bufferSize) == - 1)
        helperDebug(daemonMode_,"Error writing the msg!");

    while(!socket_.waitForBytesWritten());
}

bool Client::waitForEncryption()
{
    return socket_.waitForEncrypted(60000);
}

void Client::start()
{
    this->moveToThread(thread_);
    thread_->start();
}

void Client::die()
{
    socket_.disconnectFromHost();
    thread_->quit();
}

void Client::errorPaso(QAbstractSocket::SocketError aError)
{
    qDebug() << aError;
}
