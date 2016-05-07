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
        {"db","Set dbname","db","qversare.sqlite"},
        {"secure","Set secure mode"}
    });

    parser.addOptions(options);


    parser.parse(a->arguments());
    if( !settings.contains("ip") || !settings.contains("port") ||
       !settings.contains("db") || !settings.contains("secure") )
        setDefaultSettings(&settings);

    ( parser.isSet("ip") ) ? ipAddress_ = parser.value("ip") :
        ipAddress_ = settings.value("ip","127.0.0.1").toString();

    (parser.isSet("port") ) ? port_ = parser.value("port").toShort() :
        port_ = settings.value("port", 8000).toString().toUInt();

    (parser.isSet("db") ) ? dbName_ = parser.value("db") :
            dbName_ = settings.value("db", "qversare.sqlite").toString();

    //For server should be a bit complex
    (parser.isSet("secure") ) ? secure_ = true :
            secure_ = settings.value("secure", false).toBool();

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

bool ServerSettings::getSecure() const
{
    return secure_;
}

void ServerSettings::setDefaultSettings(QSettings *settings)
{
    settings->setValue("ip", "127.0.0.1");
    settings->setValue("port",8000);
    settings->setValue("db","qversare.sqlite");
    settings->setValue("secure",false);
}


