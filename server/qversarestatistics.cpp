#include <QTextStream>

#include "qversarestatistics.h"

QVersareStatistics::QVersareStatistics(bool daemonMode)
{
    daemonMode_ = daemonMode;
    clearCounters();
}

QVersareStatistics::~QVersareStatistics()
{
    storeMedias(calculateMedias());
}

QList<float> QVersareStatistics::calculateMedias()
{
    QList<float> medias;
    medias.append(loginCounter/loginTimes);
    medias.append(addMessageCounter/addMessageTimes);
    medias.append(avatarCounter/avatarUpdateTimes);
    return medias;
}

void QVersareStatistics::storeMedias(QList<float> medias)
{
    QList<QString> aux;
    aux.append("Login media time: ");
    aux.append("Add message to BBDD time: ");
    aux.append("Avatar update media on ddbb: ");

    if (statsFile_.open(QIODevice::WriteOnly) ) {
        QTextStream writeFile(&statsFile_);
        for (int i = 0; i < medias.size(); i++) {
            writeFile << aux.at(i) << medias.at(i) << "\n";
        }
        statsFile_.close();
    } else {
        helperDebug(daemonMode_, "Couldn't open stadistic file!!");
    }
}

void QVersareStatistics::clearCounters()
{
    loginCounter = 0;
    loginTimes = 0;
    addMessageCounter = 0;
    addMessageTimes = 0;

}

void QVersareStatistics::recordLogin(qint32 timeElapsed)
{
    loginCounter += timeElapsed;
    loginTimes++;
}

void QVersareStatistics::recordMessageAdded(qint32 timeElapsed)
{
    addMessageCounter += timeElapsed;
    loginTimes++;
}

void QVersareStatistics::avatarUpdated(qint32 timeElapsed)
{
    avatarCounter += timeElapsed;
    avatarUpdateTimes++;
}
