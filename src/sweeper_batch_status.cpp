#include "../inc/sweeper_batch_status.h"

SweeperBatchStatus::SweeperBatchStatus(QObject* parent) : QObject(parent)
{
    batchState = SweeperBatchStatus::NOT_STARTED;
    gamesCompleted = 0;
    gamesWon = 0;
    totalGames = 1;
}

/*
SweeperBatchStatus::SweeperBatchStatus(SweeperBatchStatus* batchStatus, QObject* parent) : QObject(parent)
{
    batchState = batchStatus->batchState;
    gamesCompleted  = batchStatus->batchState;
    gamesWon = batchStatus->gamesWon;
    totalGames = batchStatus->totalGames;
}
*/

void SweeperBatchStatus::copyTo(SweeperBatchStatus* batchStatus)
{
    batchStatus->batchState = batchState;
    batchStatus->gamesCompleted = gamesCompleted;
    batchStatus->gamesWon = gamesWon;
    batchStatus->totalGames = totalGames;
}
