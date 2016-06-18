#include <QTextStream>
#include <QString>
#include <QVectorIterator>

#include "qversarestatistics.h"

QVersareStatistics::QVersareStatistics(bool daemonMode, qint32 interval) :
    statsFile_("/var/lib/qVersareServer/stats.txt")
{
    daemonMode_ = daemonMode;
    if (!statsFile_.open(QIODevice::Append))
        helperDebug(daemonMode_, "Couldn't create stats file!");

    statsFile_.write("New stats \n");
    statsFile_.close();

    prepareMediasTitles();
    clearCounters();

    myTimer_.setInterval(interval);
    myTimer_.start();

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
    QVectorIterator<QPair<qint32,qint32>> auxIterator(timesAndCounters);

    while (auxIterator.hasNext()) {
        QPair<qint32,qint32> tmp = auxIterator.next();
        if(tmp.second > 0) {
            medias.push_back(tmp.first / tmp.second);
        } else {
            medias.push_back(0);
        }
    }

    return medias;
}

void QVersareStatistics::storeMedias(QList<float> medias)
{

    if (statsFile_.open(QIODevice::Append) ) {
        QTextStream writeFile(&statsFile_);
        for (int i = 0; i < medias.size(); i++) {
            writeFile << mediasTitles_.at(i) << medias.at(i) << "ms   \n";
        }
        statsFile_.close();
    } else {
        helperDebug(daemonMode_, "Couldn't open stadistic file!!");
    }
}

void QVersareStatistics::clearCounters()
{
    timesAndCounters.clear();
    QPair<qint32,qint32> aux;
    aux.first = 0;
    aux.second = 0;

    for (int i = 0; i < mediasTitles_.size(); i++)
        timesAndCounters.push_back(aux);
}

void QVersareStatistics::recordLogin(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::login);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::login, aux);
}

void QVersareStatistics::recordMessageAdded(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::addMessage);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::addMessage, aux);
}

void QVersareStatistics::recordParseTime(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::parse);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::parse, aux);
}

void QVersareStatistics::recordForward(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::forward);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::forward, aux);
}

void QVersareStatistics::recordLastTenMessages(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::lastTen);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::lastTen, aux);
}

void QVersareStatistics::recordTimeStamps(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::timeStamps);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::timeStamps, aux);
}

void QVersareStatistics::avatarUpdated(qint32 timeElapsed)
{
    QPair<qint32,qint32> aux;
    aux = timesAndCounters.takeAt(statsTypes::avatarUpdates);
    aux.first += timeElapsed;
    aux.second++;
    timesAndCounters.insert(statsTypes::avatarUpdates, aux);
}

void QVersareStatistics::onTimerTimeout()
{
    helperDebug(daemonMode_,"Writing medias to file");
    storeMedias(calculateMedias());
    clearCounters();
}
