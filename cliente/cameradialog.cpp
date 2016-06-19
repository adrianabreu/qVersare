#include "cameradialog.h"
#include "ui_cameradialog.h"
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVBoxLayout>

cameradialog::cameradialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cameradialog)
{
    ui->setupUi(this);
    mCamera_ = new QCamera(this);
    mCameraViewfinder_ = new QCameraViewfinder(this);
    mCameraImageCapture_ = new QCameraImageCapture(mCamera_, this);
    mLayout_ = new QVBoxLayout;

    mCamera_->setViewfinder(mCameraViewfinder_);
    mLayout_->addWidget(mCameraViewfinder_);
    mLayout_->setMargin(0);
    ui->scrollArea->setLayout(mLayout_);
}

cameradialog::~cameradialog()
{
    delete ui;
}

void cameradialog::setfileName(QString filename)
{
    fileName_ = filename;
}

void cameradialog::on_onPushButton_clicked()
{
    mCamera_->start();
}

void cameradialog::on_captPushButton_clicked()
{
    mCameraImageCapture_->setCaptureDestination(
                QCameraImageCapture::CaptureToFile);
    QImageEncoderSettings imageEncoderSettings;
    //imageEncoderSettings.setCodec("image/jpeg");
    //imageEncoderSettings.setResolution(100, 100);
    //mCameraImageCapture->setEncodingSettings(imageEncoderSettings);
    mCamera_->setCaptureMode(QCamera::CaptureStillImage);
    mCamera_->start();
    mCamera_->searchAndLock();
    if(mCameraImageCapture_->isReadyForCapture()) {
        mCameraImageCapture_->capture(fileName_);
        mCamera_->unlock();
        emit_load_data(fileName_);

    } else {
        mCamera_->unlock();
        mCamera_->stop();
    }
}
