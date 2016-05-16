#include "loaddialog.h"
#include "ui_loaddialog.h"

loadDialog::loadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadDialog)
{
    ui->setupUi(this);
}

loadDialog::~loadDialog()
{
    delete ui;
}
