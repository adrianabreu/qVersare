#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

#include <QString>
#include <QSettings>
#include <QCoreApplication>
//The goal of this class is to store all the settings value, coming from
//the terminal or from the file.

class ServerSettings
{
public:
    ServerSettings(QCoreApplication *a);


    QString getIpAddress() const;


    quint16 getPort() const;

private:
    void setDefaultSettings(QSettings *settings);

    QString ipAddress_;
    quint16 port_;

};

#endif // SERVERSETTINGS_H
