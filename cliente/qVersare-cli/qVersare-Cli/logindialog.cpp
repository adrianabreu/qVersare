#include "logindialog.h"
#include "ui_logindialog.h"
#include "qmessagebox.h"
#include "qsettings.h"
#include "simplecrypt.h"

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
            SimpleCrypt crypto;
            crypto.setKey(0x02ad4a4acb9f023);
            QString encrypted = crypto.encryptToString(ui->passUser->text());

            //prueba
            QMessageBox aux;
            aux.setText(encrypted);
            aux.exec();
            /*QString decrypted = crypto.decryptToString(encrypted);
            aux.setText(decrypted);
            aux.exec();*/



            settings.setValue("userName", ui->userName->text());
            settings.setValue("passUser", encrypted);

            QDialog::done(r);
            return;
        }
    } else {
        QDialog::done(r);
    }
}
