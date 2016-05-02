/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_logindialog
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *userName;
    QLabel *label_2;
    QLineEdit *passUser;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *logindialog)
    {
        if (logindialog->objectName().isEmpty())
            logindialog->setObjectName(QStringLiteral("logindialog"));
        logindialog->resize(386, 132);
        horizontalLayout = new QHBoxLayout(logindialog);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(logindialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        userName = new QLineEdit(logindialog);
        userName->setObjectName(QStringLiteral("userName"));

        verticalLayout->addWidget(userName);

        label_2 = new QLabel(logindialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        passUser = new QLineEdit(logindialog);
        passUser->setObjectName(QStringLiteral("passUser"));

        verticalLayout->addWidget(passUser);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(logindialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(logindialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), logindialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), logindialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(logindialog);
    } // setupUi

    void retranslateUi(QDialog *logindialog)
    {
        logindialog->setWindowTitle(QApplication::translate("logindialog", "Login", 0));
        label->setText(QApplication::translate("logindialog", "Nombre de Usuario", 0));
        label_2->setText(QApplication::translate("logindialog", "Contrase\303\261a", 0));
    } // retranslateUi

};

namespace Ui {
    class logindialog: public Ui_logindialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
