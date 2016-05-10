#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

#include <QCoreApplication>
#include <QSettings>
#include <QString>

//The goal of this class is to store all the settings value, coming from
//the terminal or from the file.

class ServerSettings
{
public:
    ServerSettings(QCoreApplication *a);

    bool getDaemon() const;

    QString getDbName() const;

    QString getIpAddress() const;

    quint16 getPort() const;

    bool getSecure() const;

private:
    void setDefaultSettings(QSettings *settings);

    bool daemon_;
    QString dbName_;
    QString ipAddress_;
    quint16 port_;

};

#endif // SERVERSETTINGS_H
