#ifndef SWEEPER_BATCH_STATUS_H
#define SWEEPER_BATCH_STATUS_H

#include <QObject>

class SweeperBatchStatus : public QObject
{
    Q_OBJECT
public:
    enum BATCH_STATE
    {
        NOT_STARTED,
        LOADING,
        IN_PROGRESS,
        STOPPING,
        COMPLETED,
        TERMINATED
    };
    BATCH_STATE batchState;
    int gamesPlayed;
    int gamesWon;
    int totalGames;
    explicit SweeperBatchStatus(QObject* parent = 0);
    SweeperBatchStatus(SweeperBatchStatus* batchStatus, QObject* parent = 0);
    void copyTo(SweeperBatchStatus* batchStatus);
};

#endif // SWEEPER_BATCH_STATUS_H
