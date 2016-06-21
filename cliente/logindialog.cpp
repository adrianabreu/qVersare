#include "logindialog.h"
#include "ui_logindialog.h"
#include "qmessagebox.h"
#include "qsettings.h"
#include <QCryptographicHash>

logindialog::logindialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logindialog)
{
    ui->setupUi(this);
    QSettings settings;
    ui->userName->setText(settings.value("userName", "").toString());
    ui->passUser->setEchoMode(QLineEdit::Password);
}

logindialog::~logindialog()
{
    delete ui;
}

void logindialog::done(int r)
{
    if(r == QDialog::Accepted) {
        if(ui->userName->text().isEmpty()) {
            QMessageBox::critical(this, "Login", "Es necesario introducir un "
                                                 "nombre de usuario");
            return;
        } else if(ui->passUser->text().isEmpty()) {
            QMessageBox::critical(this, "Login", "Es necesario introducir una "
                                                 "contraseña");
            return;
        } else {
            QSettings settings;
            QCryptographicHash crypto(QCryptographicHash::Md5);
            crypto.addData(ui->passUser->text().toUtf8());
            QString encrypted(crypto.result().toHex());
            emit emit_login_data(ui->userName->text(),encrypted);

            settings.setValue("userName", ui->userName->text());
            QDialog::done(r);
            return;
        }
    } else {
        QDialog::done(r);
    }
}
