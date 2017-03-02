#include "../inc/sweeper_batch_status.h"

SweeperBatchStatus::SweeperBatchStatus(QObject* parent) : QObject(parent)
{
    batchState = SweeperBatchStatus::NOT_STARTED;
    gamesInProgress = 0;
    gamesPlayed = 0;
    gamesWon = 0;
    gamesUnfinished = 0;
    totalGames = 0;
}

SweeperBatchStatus::SweeperBatchStatus(SweeperBatchStatus* batchStatus, QObject* parent) : QObject(parent)
{
    batchState = batchStatus->batchState;
    gamesInProgress = batchStatus->gamesInProgress;
    gamesPlayed = batchStatus->gamesPlayed;
    gamesWon = batchStatus->gamesWon;
    gamesUnfinished = batchStatus->gamesUnfinished;
    totalGames = batchStatus->totalGames;
}

void SweeperBatchStatus::copyTo(SweeperBatchStatus* batchStatus)
{
    batchStatus->batchState = batchState;
    batchStatus->gamesInProgress = gamesInProgress;
    batchStatus->gamesPlayed = gamesPlayed;
    batchStatus->gamesWon = gamesWon;
    batchStatus->gamesUnfinished = gamesUnfinished;
    batchStatus->totalGames = totalGames;
}
