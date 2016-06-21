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
    //qDebug() << "There is data!";
    //qDebug() << largeChunkSize_;
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
    qDebug() << aux.timestamp().c_str();
    //Deberiamos guardar el timestamp en un fichero junto al nombre de usuario
    sentTo(aux);
}

void Client::parseVerso(QVERSO my_verso)
{
    qDebug() << "Entro en parse";
     //You Are Loggin
     if (!connected_) {
         if (my_verso.has_login() && my_verso.login() == true) {
             emit messageRecive("Welcome " + userName_);
             emit avatar(userName_);
             connected_ = true;
             qDebug() << "Pero solo soy un login";
         } else {
             emit messageRecive("Login Incorrecto");
         }

    } else {
        if (my_verso.requestavatar()) {
            QDateTime dateTime;
            QString tmpDateString = QString::fromStdString(my_verso.timestamp());
            qDebug() << dateTime.currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");
            dateTime = QDateTime::fromString(tmpDateString,
                                             "yyyy-MM-ddTHH:mm:ss");
            qDebug() << tmpDateString;
            QString username = QString::fromStdString(my_verso.username());
            QPixmap pixmap;
            //QByteArray aux(my_verso.avatar().c_str(), my_verso.avatar().length());
            //QByteArray array = QByteArray::fromBase64(aux);
            QByteArray array(my_verso.avatar().c_str());
            array = QByteArray::fromBase64(array);
            pixmap.loadFromData(array);
            if (!my_verso.avatar().empty()){
                qDebug() << "Actualizo avatar";
                emitUpdateAvatar(username, dateTime, pixmap);
            } else {
                qDebug() << "Necesito el Avatar¿?";
                emitNeedAvatar(username, dateTime);
            }
        } else {
            QString username = QString::fromStdString(my_verso.username());
            QString message = QString::fromStdString(my_verso.message());
            //Lo dejamos como qstring para imprimirlo si eso?
            QString timestamp = QString::fromStdString(my_verso.timestamp());

            emit messageRecive(username + ": " + message);
        }
    }
}

void Client::setList(QList<QPair<QString, QDateTime> > lista)
{
    list_ = lista;
}

void Client::askForAvatar(QString username)
{
    QVERSO myVerso;
    myVerso.set_username(username.toStdString());
    myVerso.set_requestavatar(true);

    qDebug() << "Lo necesito";
    sentTo(myVerso);
}
