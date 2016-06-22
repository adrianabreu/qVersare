#include "loaddialog.h"
#include "ui_loaddialog.h"
#include "qsettings.h"
#include "cameradialog.h"
#include <QDebug>

loadDialog::loadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadDialog)
{
    ui->setupUi(this);
    tempPath_.clear();
}

loadDialog::~loadDialog()
{
    delete ui;
}

void loadDialog::setFinalPath(QString filename)
{
    finalPath_ = filename;
}

void loadDialog::camAvatar(QString filename)
{
    QImage finalImage;
    bool valid = finalImage.load(filename);

    if (valid) {
        tempPath_ = filename;
        //emit emit_load_data(filename);
        finalImage = finalImage.scaledToWidth(ui->image->width(),
                                              Qt::SmoothTransformation);
        ui->image->setPixmap(QPixmap::fromImage(finalImage));
    } else {
        //Error Handling
    }
}

void loadDialog::on_searchButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Seleccionar"),
                                                    "");
    if (QString::compare(filename, QString()) != 0) {
        QImage finalImage;
        bool valid = finalImage.load(filename);

        if (valid) {
            tempPath_ = filename;
            //emit emit_load_data(filename);
            finalImage = finalImage.scaledToWidth(ui->image->width(),
                                                  Qt::SmoothTransformation);
            ui->image->setPixmap(QPixmap::fromImage(finalImage));
        } else {
            //Error Handling
        }
    }
}

void loadDialog::on_camPushButton_clicked()
{
    cameradialog camera(this);
    connect(&camera, &cameradialog::emit_load_data, this, &loadDialog::camAvatar);
    camera.setfileName(finalPath_);
    camera.exec();
}

void loadDialog::on_buttonBox_accepted()
{
    if(!tempPath_.isEmpty()) {
        QPixmap pixmap;
        pixmap.load(tempPath_);
        pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
           if ( !pixmap.save(finalPath_) )
               qDebug() << "no se guarda";
        emit_load_data(finalPath_);
    }
}
