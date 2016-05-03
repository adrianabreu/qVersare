#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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


private slots:
    void on_exitButton_clicked();

    void on_conectButton_clicked();

    void on_aboutButton_clicked();

    void on_SendTextEdit_returnPressed();

    void on_confButton_clicked();

    void readyToRead(QString aux);

private:
    Ui::MainWindow *ui;
    bool isConectedButton_;
    bool isConectedToServer_;
    Client *client_;
};

#endif // MAINWINDOW_H
