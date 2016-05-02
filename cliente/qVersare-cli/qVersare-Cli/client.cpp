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
    } else {
        result = 10;
    }
    return result;
}

