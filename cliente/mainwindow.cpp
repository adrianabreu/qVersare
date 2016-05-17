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

    ui->setupUi(this);

    ui->imageButton->setIcon(icon);
    ui->imageButton->setIconSize(pixmap.size());
    client_ = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
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
        connect(client_, &Client::messageRecive, this, &MainWindow::readyToRead);
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
        //Construir qVerso y no llamar a sentto directamente
        client_->createMessageText(line);
        ui->ReciveTextEdit->appendPlainText(line);
        ui->SendTextEdit->clear();
    }
}

void MainWindow::on_confButton_clicked()
{
    ConfDialog configure;
    configure.exec();
}

void MainWindow::readyToRead(QString read){
    ui->ReciveTextEdit->appendPlainText(read);
}

void MainWindow::send_login(QString username, QString password)
{
    client_->log_me_in(username, password);
    client_->setName(username);
}

void MainWindow::refreshAvatar(QString filename)
{
    if (client_ != nullptr) {
        QString userName = client_->getName();
        QString finalPath;
        finalPath += path_ + userName + ".jpg";
        qDebug() << finalPath;
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
    } else {
        QMessageBox::critical(this, "Avatar", "Conectate al Servidor Primero");
    }
}

void MainWindow::setAvatar(QString username)
{
    QString finalPath = path_ + username + ".jpg";
    QPixmap pixmap;
    pixmap.load(finalPath);

    QIcon icon(pixmap);
    ui->imageButton->setIcon(icon);
}


void MainWindow::on_imageButton_clicked()
{
    loadDialog fotoCargada;
    connect(&fotoCargada, &loadDialog::emit_load_data, this,
            &MainWindow::refreshAvatar);
    fotoCargada.exec();
}
