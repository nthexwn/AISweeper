#include "../inc/sweeper_batch_status.h"

SweeperBatchStatus::SweeperBatchStatus(QObject* parent) : QObject(parent)
{
    batchState = SweeperBatchStatus::NOT_STARTED;
    gamesPlayed = 0;
    gamesWon = 0;
    gamesUnfinished = 0;
    totalGames = 0;
}

void SweeperBatchStatus::copyTo(SweeperBatchStatus* batchStatus)
{
    batchStatus->batchState = batchState;
    batchStatus->gamesPlayed = gamesPlayed;
    batchStatus->gamesWon = gamesWon;
    batchStatus->totalGames = totalGames;
}
