#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>
#include <QFileDialog>


namespace Ui {
class loadDialog;
}

class loadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loadDialog(QWidget *parent = 0);
    ~loadDialog();
    void setFinalPath(QString filename);

public slots:
    void camAvatar(QString filename);

signals:
    void emit_load_data(QString filename);

private slots:
    void on_searchButton_clicked();

    void on_camPushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::loadDialog *ui;
    QString finalPath_;
    QString tempPath_;
};

#endif // LOADDIALOG_H
