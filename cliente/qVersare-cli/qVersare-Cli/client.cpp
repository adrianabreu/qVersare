#include "client.h"

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

void Client::sentTo(QString line)
{
    socket_.write(line.toUtf8());
}

void Client::recivedFrom()
{
    QByteArray buffer;
    buffer.resize(socket_.bytesAvailable());
    socket_.read(buffer.data(), buffer.size());
    emit messageRecive(QString(buffer));
    /*QString aux(socket_.readAll());
    return aux;*/
}
