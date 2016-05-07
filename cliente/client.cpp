#include <QByteArray>
#include <QDataStream>
#include <QMessageBox>

#include "client.h"
#include "QVERSO.pb.h"

Client::Client()
{
    ipServer_ = "";
    portServer_ = 0;
    userName_ = "";
}

Client::Client(QString ip, int port, QString name)
{
    ipServer_ = ip;
    portServer_ = port;
    userName_ = name;
}

Client::~Client(){

}

int Client::connectTo()
{
    int result;
    socket_.connectToHost(ipServer_, portServer_);

    if (socket_.waitForConnected(3000)) {
        result = 5;
        connect(&socket_, &QTcpSocket::readyRead, this, &Client::recivedFrom);
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

void Client::recivedFrom()
{
    qint32 buffer_size = 0;
    QVERSO my_verso;

    while(socket_.bytesAvailable() > 0){
        QByteArray algo;
        QDataStream in(&socket_);
            //Recogiendo en tamaño del paquete
         if (socket_.bytesAvailable() >= (int)( sizeof(qint32) ) &&
                 (buffer_size == 0) )
         {
             in >> buffer_size;
             //Teniendo el tamaño de paquete lo leemos del buffer
         } if ( (buffer_size != 0) && (socket_.bytesAvailable() >= buffer_size )) {
            algo=socket_.read(buffer_size);
            my_verso.ParseFromString(algo.toStdString());
            buffer_size = 0;

         }
         if (!connected_) {
             if (my_verso.has_login() && my_verso.login() == true) {
                 emit messageRecive("YOU ARE IN");
                 connected_ = true;
             } else {
                 emit messageRecive("GET OUT");
             }

         } else {
             //emit forwardMessage(QString(buffer), socket_.socketDescriptor());
         }
    }

}
