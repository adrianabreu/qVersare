#include "aboutdialog.h"
#include "confdialog.h"
#include "logindialog.h"
#include "qsettings.h"
#include "qmessagebox.h"
#include "loaddialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QCursor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isConnectedButton_(false),
    isConnectedToServer_(false)
{

    ui->setupUi(this);

    QPair<QPixmap,QSize> tmp = myAvatarManager_.getDefaultAvatar();
    QIcon icon(tmp.first);

    ui->imageButton->setIcon(icon);
    ui->imageButton->setIconSize(tmp.second);
    client_ = nullptr;
    connect(this, &MainWindow::emitUpdateUserList,
            this, &MainWindow::refreshLocalUser);
    connect(&myAvatarManager_, &AvatarManager::errorLoadingAvatar, this, &MainWindow::onErrorLoadingAvatar);
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::updateAvatar(QString username, QDateTime time,
                              QPixmap image, bool same)
{
    myAvatarManager_.updateAvatar(username, time, image);
    if(same)
        setAvatar(username);
}

void MainWindow::setAvatar(QString username)
{

    QPixmap pixmap;
    if (!pixmap.load(myAvatarManager_.getUserAvatarFilePath(username)))
        pixmap = myAvatarManager_.getDefaultAvatar().first;

    QIcon icon(pixmap);
    ui->imageButton->setIcon(icon);
}

void MainWindow::onErrorLoadingAvatar()
{
    QMessageBox::critical(this, "Avatar", "Error Cargando el Avatar");
}

QString MainWindow::paddingHtmlWithSpaces(int number)
{
    QString aux = "";
    if (number > 0) {

        for(int i = 0; i < number; i++) {
            aux +="&nbsp;";
        }
    }
    return aux;
}

void MainWindow::refreshLocalUser(QString username, QDateTime time)
{
    myAvatarManager_.refreshLocalUser(username, time);
}


void MainWindow::on_exitButton_clicked()
{
    qApp->quit();
}

void MainWindow::on_connectButton_clicked()
{
    if(isConnectedButton_) {
        ui->connectButton->setText("Conectar");
        ui->ReciveTextEdit->clear();
        delete client_;
        isConnectedButton_ = false;
        isConnectedToServer_ = false;
        myAvatarManager_.updateFile();
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
            ui->connectButton->setText("Conectar");
            isConnectedButton_ = false;
            isConnectedToServer_ = false;
        } else {
            ui->connectButton->setText("Desconectar");
            isConnectedButton_ = true;
            logindialog login;
            connect(&login, &logindialog::emit_login_data, this,
                    &MainWindow::sendLogin);
            login.exec();
            client_->setActualRoom("lobby");
            isConnectedToServer_ = true;
        }
        client_->setList(myAvatarManager_.getLista());
        client_->setBasicPath(myAvatarManager_.getPath());
        connect(client_, &Client::messageReceived,
                this, &MainWindow::readyToRead);
        connect(client_, &Client::emitNeedAvatar,
                this, &MainWindow::needAvatar);
        connect(client_, &Client::emitUpdateAvatar,
                this, &MainWindow::updateAvatar);
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


    QRegExp myExp("(^/\\w+)");
    if (line.contains(myExp)) {
        line.remove(0,1);
        client_->setActualRoom(line);
        client_->sendUpdateRoom(line);
        ui->ReciveTextEdit->clear();
        ui->SendTextEdit->clear();


    } else {
        line += "\n";
        //Construir qVerso y cambiar el ultimo usuario por el que envia
        QDateTime currentTime = currentTime.currentDateTime();
        client_->createMessageText(line, currentTime);
        QString tmp;
        //Si no somos el ultimo en hablar, mostrar nuestro avatar
        //currentTime.toString("dd-MM HH:mm")

        if(client_->getLastUser() != client_->getName()) {
            client_->setLastUser(client_->getName());
            //¿Donde está el avatar?
            tmp = "<img src='" + myAvatarManager_
                    .getUserAvatarFilePath(client_->getName()) +
                    "' width='40'> <b>" +
                    client_->getName() + ": </b>" + line + "  " + "<br />";
        } else {
            //"<b>" + client_->getName() + ": </b>"
            //currentTime.toString("dd-MM HH:mm")
            tmp = paddingHtmlWithSpaces(9) + line + "  " + "<br />";
        }

        ui->ReciveTextEdit->insertHtml(tmp);

        ui->ReciveTextEdit->ensureCursorVisible();

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
    ui->ReciveTextEdit->ensureCursorVisible();
}

void MainWindow::sendLogin(QString username, QString password)
{
    client_->logMeIn(username, password);
    client_->setName(username);
    QDateTime time = QDateTime::fromMSecsSinceEpoch(0);

    myAvatarManager_.checkIfUpdateList(username);

}

void MainWindow::refreshAvatar(QString filename)
{
    if (client_ != nullptr) {
        QPixmap aux(filename);
        QIcon icon(aux);
        ui->imageButton->setIcon(icon);

        QDateTime time = QDateTime::currentDateTimeUtc();
        myAvatarManager_.updateAvatar(client_->getName(), time, aux);

        client_->sendNewAvatar(aux);
    } else {
        QMessageBox::critical(this, "Avatar", "Conectate al Servidor Primero");
    }
}

void MainWindow::addUser(QString username, QDateTime time)
{
    myAvatarManager_.addUser(username, time);
}

void MainWindow::needAvatar(QString username, QDateTime time)
{
    myAvatarManager_.needAvatar(username,time, client_);
}

void MainWindow::on_imageButton_clicked()
{
    QCursor saveCursor = ui->ReciveTextEdit->cursor();
    loadDialog fotoCargada;
    connect(&fotoCargada, &loadDialog::emit_load_data, this,
            &MainWindow::refreshAvatar);
    if (client_ != nullptr) {
        QString userName = client_->getName();
        QString finalPath;
        finalPath += "/tmp/" + userName + ".jpg";
        fotoCargada.setFinalPath(finalPath);
        fotoCargada.exec();
        ui->ReciveTextEdit->setCursor(saveCursor);
    } else {
        QMessageBox::critical(this, "Avatar", "Conectate al Servidor Primero");
    }
}
