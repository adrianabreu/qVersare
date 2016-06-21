#include "aboutdialog.h"
#include "confdialog.h"
#include "logindialog.h"
#include "qsettings.h"
#include "qmessagebox.h"
#include "loaddialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isConectedButton_(false),
    isConectedToServer_(false)
{
    path_ = QString::fromUtf8(getenv("HOME"));
    path_ += "/.local/share/qVersare/";
    QPixmap pixmap;
    pixmap.load(path_ + "qVersareDefaultAvatar.jpg");
    QIcon icon(pixmap);
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

    ui->setupUi(this);

    ui->imageButton->setIcon(icon);
    ui->imageButton->setIconSize(pixmap.size());
    client_ = nullptr;
    connect(this, &MainWindow::emitUpdateUserList, this, &MainWindow::refreshLocalUser);
    myTimer_.setInterval(300000);
    myTimer_.start();
    connect(&myTimer_,&QTimer::timeout,this,&MainWindow::updateFile);
}

MainWindow::~MainWindow()
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

    delete ui;
}

void MainWindow::updateAvatar(QString username, QDateTime time, QPixmap image, bool same)
{
    int aux = searchUser(username);
    if(aux != 15000) {
        lista_[aux].second = time;
        image.scaled(100,100,Qt::KeepAspectRatio);
        if ( !image.save(path_ + username + ".jpg") )
            qDebug() << "no se guarda";
        if(same)
            setAvatar(username);

    }
}

void MainWindow::updateFile()
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

void MainWindow::on_exitButton_clicked()
{
    qApp->quit();
}

void MainWindow::on_conectButton_clicked()
{
    if(isConectedButton_) {
        ui->conectButton->setText("Conectar");
        ui->ReciveTextEdit->clear();
        delete client_;
        isConectedButton_ = false;
        isConectedToServer_ = false;
        updateFile();
    } else {
        QSettings settings;
        QString ip = settings.value("serverAddress").toString();
        int port = settings.value("serverPort").toInt();
        client_ = new Client (ip, port);
        connect(client_, &Client::avatar, this, &MainWindow::setAvatar);
        int result = client_->connectTo();
        if (result == 10) {
            QMessageBox::critical(this, "Conectar", "Host inacesible o datos "
                                                    "incorrectos");
            ui->conectButton->setText("Conectar");
            isConectedButton_ = false;
            isConectedToServer_ = false;
        } else {
            ui->conectButton->setText("Desconectar");
            isConectedButton_ = true;
            logindialog login;
            connect(&login, &logindialog::emit_login_data, this,
                    &MainWindow::send_login);
            login.exec();
            client_->setActualRoom("lobby");
            isConectedToServer_ = true;
        }
        client_->setList(lista_);
        client_->setBasicPath(path_);
        connect(client_, &Client::messageRecive, this, &MainWindow::readyToRead);
        connect(client_, &Client::emitNeedAvatar, this, &MainWindow::needAvatar);
        connect(client_, &Client::emitUpdateAvatar, this, &MainWindow::updateAvatar);
    }


}
void MainWindow::on_aboutButton_clicked()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::on_SendTextEdit_returnPressed()
{
    QString line = ui->SendTextEdit->text();
    line += "\n";

    QRegExp myExp("(^/\\w+)");
    if (line.contains(myExp)) {
        line.remove(0,1);
        client_->setActualRoom(line);
        client_->sendUpdateRoom(line);
        ui->ReciveTextEdit->clear();
        ui->SendTextEdit->clear();


    } else {
        //Construir qVerso y no llamar a sentto directamente
        client_->createMessageText(line);
        ui->ReciveTextEdit->insertPlainText(line);
        ui->SendTextEdit->clear();
    }
}

void MainWindow::on_confButton_clicked()
{
    ConfDialog configure;
    configure.exec();
}

void MainWindow::readyToRead(QString read){
    ui->ReciveTextEdit->insertHtml(read);
}

void MainWindow::send_login(QString username, QString password)
{
    client_->log_me_in(username, password);
    client_->setName(username);
    QDateTime time = QDateTime::fromMSecsSinceEpoch(0);
    int aux = searchUser(username);
    if(aux == 15000) {
        emitUpdateUserList(username, time);
    }

}

void MainWindow::refreshAvatar(QString filename)
{
    if (client_ != nullptr) {
        QString userName = client_->getName();
        QString finalPath;
        finalPath += path_ + userName + ".jpg";
        QPixmap pixmap;
        if ( !pixmap.load(filename) ) {
            QMessageBox::critical(this, "Avatar", "Error Cargando el Avatar");
        } else {
            pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
            if ( !pixmap.save(finalPath) )
                qDebug() << "no se guarda";
        }
        QIcon icon(pixmap);

        ui->imageButton->setIcon(icon);
        QDateTime time = QDateTime::currentDateTimeUtc();
        emitUpdateUserList(userName,time);
        client_->sendNewAvatar(pixmap);
    } else {
        QMessageBox::critical(this, "Avatar", "Conectate al Servidor Primero");
    }
}

void MainWindow::setAvatar(QString username)
{
    QString finalPath = path_ + username + ".jpg";
    QPixmap pixmap;
    if (!pixmap.load(finalPath)) {
        pixmap.load(path_ + "qVersareDefaultAvatar.jpg");
    }

    QIcon icon(pixmap);
    ui->imageButton->setIcon(icon);
}

void MainWindow::addUser(QString username, QDateTime time)
{
    lista_.append(QPair<QString, QDateTime>(username, time));
}

int MainWindow::searchUser(QString username)
{
    for( int auxiliar=0; auxiliar < lista_.size(); auxiliar++)
    {
        if(lista_[auxiliar].first == username) {
            if(username == client_->getName()) {
                QPixmap pixmap;
                if (!pixmap.load(path_ + username + ".jpg")) {
                    QDateTime time = QDateTime::fromMSecsSinceEpoch(0);
                    lista_[auxiliar].second = time;
                }
            }
            return auxiliar;
        }
    }
    return 15000;
}

void MainWindow::refreshLocalUser(QString username, QDateTime time)
{
    int localizacion = searchUser(username);
    if (localizacion != 15000) {
        if (lista_[localizacion].second.operator < (time))
            lista_[localizacion].second = time;
    } else {
        addUser(username, time);
    }
}

void MainWindow::needAvatar(QString username, QDateTime time)
{
    int localizacion = searchUser(username);
    if (localizacion != 15000) {
        qDebug() << "Estaba alli";
        qDebug() << time.toString("yyyy-MM-ddTHH:mm:ss");
        if (lista_[localizacion].second.operator <(time))
            client_->askForAvatar(username);
    } else {
        qDebug() << "No estaba alli";
        addUser(username, time);
        client_->askForAvatar(username);
    }
}

void MainWindow::on_imageButton_clicked()
{
    loadDialog fotoCargada;
    connect(&fotoCargada, &loadDialog::emit_load_data, this,
            &MainWindow::refreshAvatar);
    if (client_ != nullptr) {
        QString userName = client_->getName();
        QString finalPath;
        finalPath += "/tmp/" + userName + ".jpg";
        fotoCargada.setFinalPath(finalPath);
        fotoCargada.exec();

    } else {
        QMessageBox::critical(this, "Avatar", "Conectate al Servidor Primero");
    }
}
