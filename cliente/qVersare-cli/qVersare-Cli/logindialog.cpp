#include "logindialog.h"
#include "ui_logindialog.h"

logindialog::logindialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logindialog)
{
    ui->setupUi(this);
}

logindialog::~logindialog()
{
    delete ui;
}
