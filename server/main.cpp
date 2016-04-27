#include <QCoreApplication>

#include "qversareserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("ULL");
    QCoreApplication::setOrganizationDomain("ull.es");
    QCoreApplication::setApplicationName("qVersare");


    QVersareServer myServer;
    myServer.startServer();

    return a.exec();
}
