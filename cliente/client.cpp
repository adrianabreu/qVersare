#include <QByteArray>
#include <QDataStream>
#include <QMessageBox>
#include <QBuffer>

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
    buffer_.clear();
    largeChunkSize_ = 0;
    lastUser_= "";
}

Client::Client(QString ip, int port)
{
    socket_.setPeerVerifyMode(QSslSocket::VerifyPeer);
    ipServer_ = ip;
    portServer_ = port;
    userName_ = "";
    actualRoom_ = "";
    connected_ = false;
    buffer_.clear();
    largeChunkSize_ = 0;
    lastUser_ = "";
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
    sendTo(myVerso);
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

void Client::sendTo(QVERSO aVerso)
{
    std::string buffer;
    aVerso.SerializeToString(&buffer);
    quint32 bufferSize = buffer.size();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)bufferSize;

    socket_.write(block);

    socket_.write(buffer.c_str(), bufferSize);
}

void Client::logMeIn(QString username, QString password)
{
    QVERSO logMessage;
    logMessage.set_login(true);
    logMessage.set_username(username.toStdString());
    logMessage.set_password(password.toStdString());

    sendTo(logMessage);
}

void Client::createMessageText(QString textLine, QDateTime timestamp)
{
    QVERSO myVerso;
    myVerso.set_username(userName_.toStdString());
    myVerso.set_room(actualRoom_.toStdString());
    myVerso.set_message(textLine.toStdString());
    myVerso.set_timestamp(timestamp.toString("yyyy-MM-ddTHH:mm:ss")
                          .toStdString());

    sendTo(myVerso);
}

void Client::recivedFrom()
{
    qint32 bufferSize = 0;
    QVERSO aVerso;
    //First we should check if we are reading something from and older chunk
    QByteArray tmp;
    QDataStream in(&socket_);
    bool something;

    if (largeChunkSize_ > 0) {
       if(socket_.bytesAvailable() >= largeChunkSize_) {
          buffer_ += socket_.read(largeChunkSize_);
          largeChunkSize_ = 0;
          aVerso.ParseFromString(buffer_.toStdString());
          parseVerso(aVerso);
          buffer_.clear();
       } else {
          largeChunkSize_ -= socket_.bytesAvailable();
          buffer_ += socket_.readAll();
       }
    } else {
        while (socket_.bytesAvailable() > 0){
            if (socket_.bytesAvailable() >= (int)( sizeof(qint32) ) &&
            (bufferSize == 0) ) {
                in >> bufferSize;
            }

            if ( (bufferSize != 0) &&
                 (socket_.bytesAvailable() >= bufferSize )) {
                tmp = socket_.read(bufferSize);
                something = true;
                bufferSize = 0;
            } else if (bufferSize > socket_.bytesAvailable()){
                largeChunkSize_ = bufferSize - socket_.bytesAvailable();
                buffer_ += socket_.readAll();

            } else {
               socket_.readAll();
            }
            if(something) {
                something = false;
                aVerso.ParseFromString(tmp.toStdString());
                parseVerso(aVerso);
            }
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

QString Client::getLastUser() const
{
    return lastUser_;
}

void Client::setLastUser(const QString &lastUser)
{
    lastUser_ = lastUser;
}

void Client::setActualRoom(const QString &actualRoom)
{
    actualRoom_ = actualRoom;
}

void Client::setName(const QString &name)
{
    userName_ = name;
}

void Client::setBasicPath(QString path)
{
    basicPath_ = path;
}

QString Client::getName()
{
    return userName_;
}

void Client::sendNewAvatar(QPixmap pixmap)
{
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    QVERSO aux;
    aux.set_username(userName_.toStdString());
    aux.set_requestavatar(true);
    aux.set_avatar(bArray.toBase64().toStdString());
    aux.set_room(actualRoom_.toStdString());
    QDateTime time;
    aux.set_timestamp(time.currentDateTime()
                      .toString("yyyy-MM-ddTHH:mm:ss")
                      .toStdString());

    sendTo(aux);
}

void Client::parseVerso(QVERSO myVerso)
{
     if (!connected_) {
         if (myVerso.has_login() && myVerso.login() == true) {
             QString htmlMessage = "<h1 align=center>Welcome " + userName_ + "</h1><br />";
             emit messageReceived(htmlMessage);
             emit avatar(userName_);
             connected_ = true;

         } else {
             QString htmlMessage = "<h2> Login Incorrecto </h2><br />";
             emit messageReceived(htmlMessage);
         }

    } else {
        if (myVerso.requestavatar()) {

            QDateTime dateTime;
            QString tmpDateString = QString::fromStdString(myVerso.timestamp());
            dateTime = QDateTime::fromString(tmpDateString,
                                             "yyyy-MM-ddTHH:mm:ss");
            QString username = QString::fromStdString(myVerso.username());
            QPixmap pixmap;
            QByteArray array(myVerso.avatar().c_str());
            array = QByteArray::fromBase64(array);
            pixmap.loadFromData(array);

            bool same = (myVerso.username() == userName_.toStdString());

            if (!myVerso.avatar().empty()){
                emitUpdateAvatar(username, dateTime, pixmap, same);
            } else {
                emitNeedAvatar(username, dateTime);
            }
        } else {
            showMessage(myVerso);
        }
    }
}

void Client::setList(QList<QPair<QString, QDateTime> > lista)
{
    list_ = lista;
}

void Client::showMessage(QVERSO myVerso)
{
    //Extraer componentes del mensaje
    QString username = QString::fromStdString(myVerso.username());
    QString message = QString::fromStdString(myVerso.message());
    QString timestamp = QString::fromStdString(myVerso.timestamp());
    QString tmpDateString = QString::fromStdString(myVerso.timestamp());

    QDateTime messageTime;
    messageTime = QDateTime::fromString(tmpDateString, "yyyy-MM-ddTHH:mm:ss");

    //Preparar el mensaje
    QString htmlMessage;
    QString imageText = basicPath_ + username + ".jpg";
    //En caso de que sean usuarios distintos
    //messageTime.toString("dd-MM HH:mm")

    if(lastUser_ != username){
        lastUser_ = username;
        htmlMessage = "<img src='" + imageText+ "' height='40'> <b>" +
                username + ": </b>" + message + "  "  + "<br />";
    } else {
        for(int i = 0; i < 9; i++) {
            htmlMessage +="&nbsp;";
        };

        htmlMessage += message + "  " + "<br />";
    }
    emit messageReceived(htmlMessage);
}

void Client::askForAvatar(QString username)
{
    QVERSO myVerso;
    myVerso.set_username(username.toStdString());
    myVerso.set_requestavatar(true);

    sendTo(myVerso);
}
