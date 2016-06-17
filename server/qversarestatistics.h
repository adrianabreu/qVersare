#ifndef QVERSARESTATISTICS_H
#define QVERSARESTATISTICS_H

#include <QFile>
#include <QList>
#include <QObject>
#include <QPair>
#include <QTimer>

#include "utils.h"

class QVersareStatistics : public QObject
{
public:
    QVersareStatistics(bool daemonMode_);
    ~QVersareStatistics();

    void prepareMediasTitles();

    QList<float> calculateMedias();
    void storeMedias(QList<float> medias);

    void clearCounters();
    void recordLogin(qint32 timeElapsed);
    void recordMessageAdded(qint32 timeElapsed);
    void recordParseTime(qint32 timeElapsed);
    void recordForward(qint32 timeElapsed);
    void recordLastTenMessages(qint32 timeElapsed);
    void recordTimeStamps(qint32 timeElapsed);
    void avatarUpdated(qint32 timeElapsed);

public slots:
    void onTimerTimeout();

private:
    QFile statsFile_;
    QTimer myTimer_;
    bool daemonMode_;
    QList<QString> mediasTitles_;

    QList<QPair<qint32,qint32>> timesAndCounters;
    //Counter for add time there and times for the numbers of times
    qint32 loginTimes;
    qint32 loginCounter;

    qint32 forwardTimes;
    qint32 forwardCounter;

    qint32 avatarUpdateTimes;
    qint32 avatarCounter;

    qint32 addMessageTimes;
    qint32 addMessageCounter;

    qint32 parseTimes;
    qint32 parseCounter;

    qint32 lastTenMessagesTimes;
    qint32 lastTenMessageCounter;

    qint32 retrieveTimeStampTimes;
    qint32 retrieveTimeStampCounter;
};

#endif // QVERSARESTATISTICS_H
