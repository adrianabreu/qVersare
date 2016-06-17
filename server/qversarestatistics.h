#ifndef QVERSARESTATISTICS_H
#define QVERSARESTATISTICS_H

#include <QFile>
#include <QList>
#include <QTimer>

#include "utils.h"

class QVersareStatistics
{
public:
    QVersareStatistics(bool daemonMode_);
    ~QVersareStatistics();

    QList<float> calculateMedias();
    void storeMedias(QList<float> medias);

    void clearCounters();
    void recordLogin(qint32 timeElapsed);
    void recordMessageAdded(qint32 timeElapsed);
    void avatarUpdated(qint32 timeElapsed);

private:
    QFile statsFile_;

    bool daemonMode_;
    //Counter for add time there and times for the numbers of times
    qint32 loginTimes;
    qint32 loginCounter;

    qint32 forwardTimes;
    qint32 forwardCounter;

    qint32 avatarUpdateTimes;
    qint32 avatarCounter;

    qint32 addMessageTimes;
    qint32 addMessageCounter;

    QTimer retrieveTimeStampTimer;
    qint32 retrieveTimeStampTimes;
};

#endif // QVERSARESTATISTICS_H
