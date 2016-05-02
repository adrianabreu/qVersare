#include "confdialog.h"
#include "ui_confdialog.h"

ConfDialog::ConfDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfDialog)
{
    ui->setupUi(this);
}

ConfDialog::~ConfDialog()
{
    delete ui;
}
