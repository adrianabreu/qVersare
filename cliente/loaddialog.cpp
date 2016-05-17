#include "loaddialog.h"
#include "ui_loaddialog.h"
#include "qsettings.h"

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

void loadDialog::on_searchButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Seleccionar"),
                                                    "");
    if (QString::compare(filename, QString()) != 0) {
        QImage finalImage;
        bool valid = finalImage.load(filename);

        if (valid) {
            emit emit_load_data(filename);
            finalImage = finalImage.scaledToWidth(ui->image->width(), Qt::SmoothTransformation);
            ui->image->setPixmap(QPixmap::fromImage(finalImage));
        } else {
            //Error Handling
        }
    }
}
