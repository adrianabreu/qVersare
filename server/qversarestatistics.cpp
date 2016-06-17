#include <QTextStream>
#include <QString>
#include "qversarestatistics.h"

QVersareStatistics::QVersareStatistics(bool daemonMode) :
    statsFile_("/var/lib/qVersareServer/stats.txt")
{
    daemonMode_ = daemonMode;
    if (!statsFile_.open(QIODevice::Append))
        helperDebug(daemonMode_, "Couldn't create stats file!");

    statsFile_.write("New stats \n");
    statsFile_.close();
    clearCounters();

    myTimer_.setInterval(60000);
    myTimer_.start();

    prepareMediasTitles();

    connect(&myTimer_,&QTimer::timeout,this,
            &QVersareStatistics::onTimerTimeout);
}

QVersareStatistics::~QVersareStatistics()
{
    storeMedias(calculateMedias());
}

void QVersareStatistics::prepareMediasTitles()
{
    mediasTitles_.append("Login media time: ");
    mediasTitles_.append("Parsing media time: ");
    mediasTitles_.append("Add message to BBDD time: ");
    mediasTitles_.append("Avatar update media on ddbb: ");
    mediasTitles_.append("Forward timestamp: ");
    mediasTitles_.append("Last ten messages time: ");
    mediasTitles_.append("Others timestamp time: ");
}

QList<float> QVersareStatistics::calculateMedias()
{
    QList<float> medias;
    float aux;

    (loginTimes > 0) ? aux = loginCounter/loginTimes : aux = 0;
    medias.append(aux);
    (parseTimes > 0) ? aux = parseCounter/parseTimes : aux = 0;
    medias.append(aux);
    (addMessageTimes > 0) ? aux = (addMessageCounter/addMessageTimes) : aux = 0;
    medias.append(aux);
    (avatarUpdateTimes > 0) ? aux = (avatarCounter/avatarUpdateTimes) : aux = 0;
    medias.append(aux);
    (forwardTimes > 0) ? aux = (forwardCounter/forwardTimes) : aux = 0;
    medias.append(aux);
    (lastTenMessagesTimes > 0) ?
                aux = (lastTenMessageCounter/lastTenMessagesTimes) : aux = 0;
    medias.append(aux);
    (retrieveTimeStampTimes > 0) ?
                aux = (retrieveTimeStampCounter/retrieveTimeStampTimes) : aux = 0;
    medias.append(aux);
    return medias;
}

void QVersareStatistics::storeMedias(QList<float> medias)
{

    if (statsFile_.open(QIODevice::Append) ) {
        QTextStream writeFile(&statsFile_);
        for (int i = 0; i <= medias.size(); i++) {
            writeFile << mediasTitles_.at(i) << medias.at(i) << "ms   \n";
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
    parseCounter = 0;
    parseTimes = 0;
    avatarCounter = 0;
    avatarUpdateTimes = 0;
    forwardCounter = 0;
    forwardTimes = 0;
    lastTenMessageCounter = 0;
    lastTenMessagesTimes = 0;
    retrieveTimeStampCounter = 0;
    retrieveTimeStampTimes = 0;
}

void QVersareStatistics::recordLogin(qint32 timeElapsed)
{
    loginCounter += timeElapsed;
    loginTimes++;
}

void QVersareStatistics::recordMessageAdded(qint32 timeElapsed)
{
    addMessageCounter += timeElapsed;
    addMessageTimes++;
}

void QVersareStatistics::recordParseTime(qint32 timeElapsed)
{
    parseCounter += timeElapsed;
    parseTimes++;
}

void QVersareStatistics::recordForward(qint32 timeElapsed)
{
    forwardCounter += timeElapsed;
    forwardTimes++;
}

void QVersareStatistics::recordLastTenMessages(qint32 timeElapsed)
{
    lastTenMessageCounter += timeElapsed;
    lastTenMessagesTimes++;
}

void QVersareStatistics::recordTimeStamps(qint32 timeElapsed)
{
    retrieveTimeStampCounter += timeElapsed;
    retrieveTimeStampTimes++;
}

void QVersareStatistics::avatarUpdated(qint32 timeElapsed)
{
    avatarCounter += timeElapsed;
    avatarUpdateTimes++;
}

void QVersareStatistics::onTimerTimeout()
{
    helperDebug(daemonMode_,"Writing medias to file");
    storeMedias(calculateMedias());
    clearCounters();
}
