#include "aboutdialog.h"
#include "confdialog.h"
#include "logindialog.h"
#include "qsettings.h"
#include "qmessagebox.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isConectedButton_(false),
    isConectedToServer_(false)
{
    ui->setupUi(this);
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
        isConectedButton_ = false;
        isConectedToServer_ = false;
    } else {
        logindialog login;
        login.exec();
        QSettings settings;
        QString userName = settings.value("userName").toString();
        QString ip = settings.value("serverAddress").toString();
        int port = settings.value("serverPort").toInt();
        client_ = new Client (ip, port, userName);
        int result = client_->connectTo();
        if (result == 10) {
            QMessageBox::critical(this, "Conectar", "Host inacesible o datos "
                                                    "incorrectos");
            ui->conectButton->setText("Conectar");
            isConectedButton_ = false;
            isConectedToServer_ = false;
        } else {
            QMessageBox text;
            text.setText("Conectado correctamente");
            text.exec();
            ui->conectButton->setText("Desconectar");
            isConectedButton_ = true;
            isConectedToServer_ = true;
        }
    }
    connect(client_, &Client::messageRecive, this, &MainWindow::readyToRead);

}
void MainWindow::on_aboutButton_clicked()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::on_SendTextEdit_returnPressed()
{
    QString line = ui->SendTextEdit->text() + '\n';
    client_->sentTo(line);
    ui->ReciveTextEdit->appendPlainText(line);
    ui->SendTextEdit->clear();
}

void MainWindow::on_confButton_clicked()
{
    ConfDialog configure;
    configure.exec();
}

void MainWindow::readyToRead(QString read){
    ui->ReciveTextEdit->appendPlainText(read);
}
