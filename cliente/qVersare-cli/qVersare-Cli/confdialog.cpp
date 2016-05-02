#include "confdialog.h"
#include "ui_confdialog.h"
#include "qmessagebox.h"
#include "qsettings.h"

ConfDialog::ConfDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfDialog)
{
    ui->setupUi(this);
    QSettings settings;
    ui->serverAddress->setText(settings.value("serverAddress", "").toString());
    ui->serverPort->setValue(settings.value("serverPort", 8000).toInt());
}

ConfDialog::~ConfDialog()
{
    delete ui;
}

void ConfDialog::done(int r)
{
    if(r == QDialog::Accepted) {
        if(ui->serverAddress->text().isEmpty()) {
            QMessageBox::critical(this, "Configuracion",
                                  "Se necesita una direccion IP");
            return;
        } else {
            QSettings settings;
            settings.setValue("serverAddress", ui->serverAddress->text());
            settings.setValue("serverPort", ui->serverPort->text());

            QDialog::done(r);
            return;
        }
    } else {
        QDialog::done(r);
    }
}
