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
        COMPLETED,
        TERMINATING,
        TERMINATED
    };
    BATCH_STATE batchState;
    int gamesInProgress;
    int gamesPlayed;
    int gamesWon;
    int gamesUnfinished;
    int totalGames;
    explicit SweeperBatchStatus(QObject* parent = 0);
    SweeperBatchStatus(SweeperBatchStatus* batchStatus, QObject* parent = 0);
    void copyTo(SweeperBatchStatus* batchStatus);
};

#endif // SWEEPER_BATCH_STATUS_H
