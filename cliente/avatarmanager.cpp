#include "avatarmanager.h"

AvatarManager::AvatarManager()
{
    path_ = QString::fromUtf8(getenv("HOME"));
    path_ += "/.local/share/qVersare/";

    file_ = new QFile(path_ + "avatarList.txt");
    if(file_->open(QIODevice::ReadWrite)) {
        QTextStream stream(file_);
        QStringList line;
        while(!stream.atEnd()) {
            line = stream.readLine().split(',');
            QDateTime temp = QDateTime::fromString(line[1],"yyyy-MM-ddTHH:mm:ss");
            lista_.append(QPair<QString, QDateTime>(line[0], temp));
        }
    }
    file_->close();

    myTimer_.setInterval(300000);
    myTimer_.start();
    connect(&myTimer_,&QTimer::timeout,this,&AvatarManager::updateFile);
}

AvatarManager::~AvatarManager()
{
    file_->remove(path_ + "avatarList.txt");
    file_ = new QFile(path_ + "avatarList.txt");
    if(file_->open(QIODevice::ReadWrite)) {
        QTextStream stream(file_);
        for(int aux = 0; aux < lista_.size(); aux++)
        {
            stream << lista_[aux].first << "," <<
                      lista_[aux].second.toString("yyyy-MM-ddTHH:mm:ss") << "\n";
        }
    }
}

QPair<QPixmap,QSize> AvatarManager::getDefaultAvatar()
{
    QPair<QPixmap,QSize> aux;
    QPixmap pixmap;
    pixmap.load(path_ + "qVersareDefaultAvatar.jpg");
    aux.first = pixmap;
    aux.second = pixmap.size();
    return aux;
}

QPixmap AvatarManager::getUserAvatar(QString username)
{
    QString finalPath;
    finalPath += path_ + username + ".jpg";
    QPixmap pixmap;
    if ( !pixmap.load(finalPath) ) {
        emit errorLoadingAvatar();
    } else {
        pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
        if ( !pixmap.save(finalPath) )
            qDebug() << "No se guarda el avatar";
    }
    return pixmap;
}

void AvatarManager::updateUserList(QString username, QDateTime time)
{
    //Actualizamos la lista
    int aux = searchUser(username);

    if(aux != 15000) {
        lista_[aux].second = time;
    } else {
        QPair<QString,QDateTime> auxPair;
        auxPair.first = username;
        auxPair.second = time;
        lista_.append(auxPair);
    }
}

void AvatarManager::updateFile()
{
    //Borramos el antiguo
    file_->remove(path_ + "avatarList.txt");

    //Creamos el nuevo
    file_ = new QFile(path_ + "avatarList.txt");
    if(file_->open(QIODevice::ReadWrite)) {
        QTextStream stream(file_);
        for(int aux = 0; aux < lista_.size(); aux++)
        {
            stream << lista_[aux].first << "," <<
                      lista_[aux].second.toString("yyyy-MM-ddTHH:mm:ss") << "\n";
        }
    }

}

void AvatarManager::checkIfUpdateList(QString username)
{
    int aux = searchUser(username);
    //Si no lo encuentra
    if(aux == 15000) {
        updateUserList(username, QDateTime::fromMSecsSinceEpoch(0));
    }
}


void AvatarManager::onTimerTimeout()
{
    updateFile();
}

QString AvatarManager::getPath() const
{
    return path_;
}

void AvatarManager::updateAvatar(QString username, QDateTime time, QPixmap pixmap)
{
    QString finalPath;
    finalPath += path_ + username + ".jpg";

    pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
       if ( !pixmap.save(finalPath) )
           qDebug() << "no se guarda";

    updateUserList(username,time);
}

QList<QPair<QString, QDateTime> > AvatarManager::getLista()
{
    return lista_;
}

void AvatarManager::addUser(QString username, QDateTime time)
{
    lista_.append(QPair<QString, QDateTime>(username, time));
}

QString AvatarManager::getUserAvatarFilePath(QString username)
{
    return path_ + username + ".jpg";
}

int AvatarManager::searchUser(QString username)
{
    for( int auxiliar=0; auxiliar < lista_.size(); auxiliar++)
    {
        if(lista_[auxiliar].first == username) {
               //Ponemos fecha por defecto, 1-1-1970
               QDateTime time = QDateTime::fromMSecsSinceEpoch(0);
               lista_[auxiliar].second = time;
               return auxiliar;

         }
    }
    return 15000;
}

int AvatarManager::searchUser(QString username, QString thisUsername)
{
    for( int auxiliar=0; auxiliar < lista_.size(); auxiliar++)
    {
        if(lista_[auxiliar].first == username) {
            if(username == thisUsername) {
                QPixmap pixmap;
                if (!pixmap.load(path_ + username + ".jpg")) {
                    //Ponemos fecha por defecto, 1-1-1970
                    QDateTime time = QDateTime::fromMSecsSinceEpoch(0);
                    lista_[auxiliar].second = time;
                }
            }
            return auxiliar;
        }
    }
    return 15000;
}

void AvatarManager::refreshLocalUser(QString username, QDateTime time)
{
    int localizacion = searchUser(username);
    if (localizacion != 15000) {
        if (lista_[localizacion].second.operator < (time))
            lista_[localizacion].second = time;
    } else {
        addUser(username, time);
    }
}

void AvatarManager::needAvatar(QString username, QDateTime time, Client *client)
{
    int localizacion = searchUser(username);
    if (localizacion != 15000) {
        if (lista_[localizacion].second.operator <(time))
            client->askForAvatar(username);
    } else {
        addUser(username, time);
        client->askForAvatar(username);
    }
}
