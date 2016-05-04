#include <QCommandLineParser>
#include <QList>
#include <QSettings>

#include "serversettings.h"


ServerSettings::ServerSettings(QCoreApplication *a)
{
    QCommandLineParser parser;
    QSettings settings;

    QList<QCommandLineOption> options(
    {
        {"ip","Set ip to listen to", "ip","127.0.0.1"},
        {"port","Set port to listen to","port","9000"},
        {"db","Set dbname","db","users.db"}
    });

    parser.addOptions(options);


    parser.parse(a->arguments());
    if(!settings.contains("ip") || (!settings.contains("port") ||
                                    (!settings.contains("db")) ) )
        setDefaultSettings(&settings);

    if ( parser.isSet("ip") ) {
        ipAddress_ = parser.value("ip");
    } else {
        ipAddress_ = settings.value("ip","127.0.0.1").toString();
    }

    if (parser.isSet("port") ) {
        port_ = parser.value("port").toShort();
    } else {
        port_ = settings.value("port", 8000).toString().toUInt();
    }

    if (parser.isSet("db") ) {
        dbName_ = parser.value("db");
    } else {
        dbName_ = settings.value("db", "users.db").toString();
    }

}

QString ServerSettings::getIpAddress() const
{
    return ipAddress_;
}

quint16 ServerSettings::getPort() const
{
    return port_;
}

QString ServerSettings::getDbName() const
{
    return dbName_;
}

void ServerSettings::setDefaultSettings(QSettings *settings)
{
    settings->setValue("ip", "127.0.0.1");
    settings->setValue("port",8000);
    settings->setValue("db","users.db");
}
