#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include <QDialog>

namespace Ui {
class ConfDialog;
}

class ConfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfDialog(QWidget *parent = 0);
    ~ConfDialog();

private:
    Ui::ConfDialog *ui;
};

#endif // CONFDIALOG_H
