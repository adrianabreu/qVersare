#ifndef CAMERADIALOG_H
#define CAMERADIALOG_H

#include <QDialog>

namespace Ui {
class cameradialog;
}

class QCamera;
class QCameraViewfinder;
class QCameraImageCapture;
class QVBoxLayout;

class cameradialog : public QDialog
{
    Q_OBJECT

public:
    explicit cameradialog(QWidget *parent = 0);
    ~cameradialog();
    void setfileName(QString filename);

signals:
    void emit_load_data(QString filename);

private slots:
    void on_onPushButton_clicked();

    void on_captPushButton_clicked();

private:
    Ui::cameradialog *ui;
    QCamera *mCamera_;
    QCameraViewfinder *mCameraViewfinder_;
    QCameraImageCapture *mCameraImageCapture_;
    QVBoxLayout *mLayout_;
    QString fileName_;
};

#endif // CAMERADIALOG_H
