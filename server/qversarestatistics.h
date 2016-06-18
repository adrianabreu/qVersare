#ifndef QVERSARESTATISTICS_H
#define QVERSARESTATISTICS_H

#include <QFile>
#include <QObject>
#include <QPair>
#include <QTimer>
#include <QVector>

#include "utils.h"

class QVersareStatistics : public QObject
{
    Q_ENUMS(statsTypes)

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

    enum statsTypes
    {
        login,
        parse,
        addMessage,
        avatarUpdates,
        forward,
        lastTen,
        timeStamps
    };

public slots:
    void onTimerTimeout();

private:
    QFile statsFile_;
    QTimer myTimer_;
    bool daemonMode_;
    QVector<QString> mediasTitles_;

    //First is sum of value, second is times measured for media
    QVector<QPair<qint32,qint32>> timesAndCounters;
};

#endif // QVERSARESTATISTICS_H
