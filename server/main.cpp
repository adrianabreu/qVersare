#include <QCoreApplication>

#include "qversareserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QVersareServer myServer;
    myServer.startServer();

    return a.exec();
}
