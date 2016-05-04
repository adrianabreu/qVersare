#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class logindialog;
}

class logindialog : public QDialog
{
    Q_OBJECT

public:
    explicit logindialog(QWidget *parent = 0);
    ~logindialog();

signals:
    void emit_login_data(QString username, QString password);

public slots:
    void done(int r);

private:
    Ui::logindialog *ui;
};

#endif // LOGINDIALOG_H
