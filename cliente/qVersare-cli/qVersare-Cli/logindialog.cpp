#include "logindialog.h"
#include "ui_logindialog.h"
#include "qmessagebox.h"
#include "qsettings.h"

logindialog::logindialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logindialog)
{
    ui->setupUi(this);
    QSettings settings;
    ui->userName->setText(settings.value("userName", "").toString());
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
            settings.setValue("userName", ui->userName->text());
            settings.setValue("passUser", ui->passUser->text());

            QDialog::done(r);
            return;
        }
    } else {
        QDialog::done(r);
    }
}
