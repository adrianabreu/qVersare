#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("ULL");
    QApplication::setOrganizationName("ull.es");
    QApplication::setApplicationName("qVersare");

    MainWindow w;
    w.show();

    return a.exec();
}
