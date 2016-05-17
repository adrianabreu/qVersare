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


signals:
    void emit_load_data(QString filename);

private slots:
    void on_searchButton_clicked();

private:
    Ui::loadDialog *ui;
};

#endif // LOADDIALOG_H
