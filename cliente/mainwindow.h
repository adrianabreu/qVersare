#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegExp>
#include <QDebug>
#include <QPixmap>
#include <QFile>
#include <QPair>
#include <QList>
#include <QDateTime>
#include "client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void emitUpdateUserList(QString username, QDateTime time);


private slots:
    void on_exitButton_clicked();

    void on_conectButton_clicked();

    void on_aboutButton_clicked();

    void on_SendTextEdit_returnPressed();

    void on_confButton_clicked();

    void readyToRead(QString aux);

    void on_imageButton_clicked();

public slots:
    void send_login(QString username, QString password);

    void refreshAvatar(QString filename);

    void setAvatar(QString username);

    void addUser(QString username, QDateTime time);

    int searchUser(QString username);

    void refreshLocalUser(QString username, QDateTime time);

    void needAvatar(QString username, QDateTime time);

    void updateAvatar(QString username, QDateTime time, QPixmap image, bool same);


private:
    Ui::MainWindow *ui;
    bool isConectedButton_;
    bool isConectedToServer_;
    Client *client_;
    QString path_;
    QFile *file_;
    QList<QPair<QString, QDateTime>> lista_;

};

#endif // MAINWINDOW_H
