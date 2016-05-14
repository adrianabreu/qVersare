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

    setupSecureMode(keyPath, certPath);
}

Client::~Client()
{
    socket_.close();
}


void Client::readyRead()
{
    qint32 bufferSize = 0;
    QVERSO aVerso;
    while(socket_.bytesAvailable() > 0){
        QByteArray tmp;
        QDataStream in(&socket_);

         if (socket_.bytesAvailable() >= (int)( sizeof(qint32) ) &&
                 (bufferSize == 0) ) {
             in >> bufferSize;

         }
         if ( (bufferSize != 0) &&
                (socket_.bytesAvailable() >= bufferSize )) {
            tmp=socket_.read(bufferSize);
            aVerso.ParseFromString(tmp.toStdString());
            bufferSize = 0;

        } else {
             socket_.readAll();
        }

        parseVerso(aVerso);
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
    return socket_.waitForEncrypted();
}

void Client::parseVerso(QVERSO aVerso)
{
    if (!logged_) {
        if (aVerso.login()) {
            emit validateMe(QString::fromStdString(aVerso.username() ),
                         QString::fromStdString(aVerso.password()), this);
        }
    } else {
        if (aVerso.room() != room_.toStdString()) {
            room_ = QString::fromStdString(aVerso.room());
            emit Client::imNewInTheRoom(room_, socketFd_);
        } else {
            emit forwardMessage(aVerso,socketFd_);
        }
    }
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

void Client::readySslErrors(QList<QSslError> errors)
{
    for (auto error = errors.begin(); error != errors.end(); ++error)
        helperDebug(daemonMode_, error->errorString());
}

void Client::setupSecureMode(QString keyPath, QString certPath)
{
    //Seteamos la clave privada
    socket_.setPrivateKey(keyPath);

    QFile myCert(certPath);
    if(myCert.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QSslCertificate serverCert(myCert.readAll());
        QList<QSslCertificate> caCert;
        caCert.push_back(serverCert);
        socket_.setCaCertificates(caCert);
        myCert.close();

        //Preparamos una lista de errores por ser selfsigned
        QSslError error(QSslError::SelfSignedCertificate, caCert.at(0));
        QList<QSslError>  expectedSslErrors;
        expectedSslErrors.append(error);
        expectedSslErrors.append(QSslError::HostNameMismatch);
        socket_.ignoreSslErrors(expectedSslErrors);

        socket_.setLocalCertificate(serverCert);
        socket_.setSocketDescriptor(socketFd_);

        connect(&socket_,SIGNAL(sslErrors(QList<QSslError>)),this,
                SLOT(readySslErrors(QList<QSslError>)));

        socket_.startServerEncryption();

        helperDebug(daemonMode_,"Client connected");
    } else {
        helperDebug(daemonMode_, "Fallo al abrir el cert");
    }
}
