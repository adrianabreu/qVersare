/********************************************************************************
** Form generated from reading UI file 'confdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFDIALOG_H
#define UI_CONFDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConfDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *serverAddress;
    QLabel *label_2;
    QSpinBox *serverPort;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ConfDialog)
    {
        if (ConfDialog->objectName().isEmpty())
            ConfDialog->setObjectName(QStringLiteral("ConfDialog"));
        ConfDialog->resize(392, 119);
        verticalLayout = new QVBoxLayout(ConfDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(ConfDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        serverAddress = new QLineEdit(ConfDialog);
        serverAddress->setObjectName(QStringLiteral("serverAddress"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serverAddress);

        label_2 = new QLabel(ConfDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        serverPort = new QSpinBox(ConfDialog);
        serverPort->setObjectName(QStringLiteral("serverPort"));
        serverPort->setMinimum(80);
        serverPort->setMaximum(65525);
        serverPort->setValue(8000);

        formLayout->setWidget(1, QFormLayout::FieldRole, serverPort);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(ConfDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ConfDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConfDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConfDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConfDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfDialog)
    {
        ConfDialog->setWindowTitle(QApplication::translate("ConfDialog", "Configuraci\303\263n", 0));
        label->setText(QApplication::translate("ConfDialog", "Direcci\303\263n del Servidor", 0));
        label_2->setText(QApplication::translate("ConfDialog", "Puerto del Servidor", 0));
    } // retranslateUi

};

namespace Ui {
    class ConfDialog: public Ui_ConfDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFDIALOG_H
