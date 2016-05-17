#include <QByteArray>
#include <QDataStream>
#include <QMessageBox>

#include <QAbstractSocket>
#include <QNetworkSession>

#include "client.h"
#include "QVERSO.pb.h"

Client::Client()
{
    ipServer_ = "";
    portServer_ = 0;
    userName_ = "";
    actualRoom_ = "";
    connected_ = false;
}

Client::Client(QString ip, int port)
{
    socket_.setPeerVerifyMode(QSslSocket::VerifyPeer);
    ipServer_ = ip;
    portServer_ = port;
    userName_ = "";
    actualRoom_ = "";
    connected_ = false;
    connect(&socket_, SIGNAL(sslErrors(QList<QSslError>)), this,
            SLOT(procesarErroresSsl(QList<QSslError>)));
}

Client::~Client(){
    emit avatar("qVersareDefaultAvatar");
}

void Client::sendUpdateRoom(QString room)
{
    QVERSO myVerso;
    myVerso.set_username(userName_.toStdString());
    myVerso.set_room(room.toStdString());
    sentTo(myVerso);
}

int Client::connectTo()
{
    int result;
    socket_.connectToHostEncrypted(ipServer_, portServer_);

    if (socket_.waitForConnected(50000)) {
        result = 5;
        connect(&socket_, &QSslSocket::readyRead, this, &Client::recivedFrom);
    } else {
        result = 10;
    }

    return result;
}

void Client::sentTo(QVERSO a_verso)
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

void Client::log_me_in(QString username, QString password)
{
    QVERSO logMessage;
    logMessage.set_login(true);
    logMessage.set_username(username.toStdString());
    logMessage.set_password(password.toStdString());

    sentTo(logMessage);
}

void Client::createMessageText(QString textLine)
{
    QVERSO myVerso;
    myVerso.set_username(userName_.toStdString());
    myVerso.set_room(actualRoom_.toStdString());
    myVerso.set_message(textLine.toStdString());

    sentTo(myVerso);

}

void Client::recivedFrom()
{
    qint32 buffer_size = 0;
    QVERSO my_verso;

    while(socket_.bytesAvailable() > 0){
        QByteArray algo;
        QDataStream in(&socket_);
            //Recogiendo en tamaño del paquete
         if (socket_.bytesAvailable() >= (int)( sizeof(qint32) ) &&
                 (buffer_size == 0) ) {
             in >> buffer_size;
             //Teniendo el tamaño de paquete lo leemos del buffer
         } if ((buffer_size != 0) && (socket_.bytesAvailable() >= buffer_size)) {
            algo=socket_.read(buffer_size);
            my_verso.ParseFromString(algo.toStdString());
            buffer_size = 0;
         } else {
             socket_.readAll();
         }


         //You Are Loggin
         if (!connected_) {
             if (my_verso.has_login() && my_verso.login() == true) {
                 emit messageRecive("Welcome " + userName_);
                 emit avatar(userName_);
                 connected_ = true;
             } else {
                 emit messageRecive("Login Incorrecto");
             }

         } else {
             QString username = QString::fromStdString(my_verso.username());
             QString message = QString::fromStdString(my_verso.message());
             emit messageRecive(username + ": " + message);
         }
    }
}

void Client::procesarErroresSsl(QList<QSslError> myList)
{
    bool aux = true;
    for(auto it = myList.begin(); it != myList.end(); ++it) {
        if (it->error() != QSslError::HostNameMismatch &&
                it->error() != QSslError::SelfSignedCertificate) {
            aux = false;
        }
    }
    if (aux)
        socket_.ignoreSslErrors(myList);
}

void Client::setActualRoom(const QString &actualRoom)
{
    actualRoom_ = actualRoom;
}

void Client::setName(const QString &name)
{
    userName_ = name;
}

QString Client::getName()
{
    return userName_;
}
