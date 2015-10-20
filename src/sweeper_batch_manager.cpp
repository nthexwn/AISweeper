#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    batchStatus = new SweeperBatchStatus();
    batchTerminationRequested = false;
}

SweeperBatchManager::~SweeperBatchManager()
{
    delete batchStatus;
    delete sweeperGame;
}

void SweeperBatchManager::launchBatch(SweeperBatchSettings* batchSettings)
{
    sweeperGame = new SweeperGame(batchSettings);
    batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
    emit triggerUpdateOverview(batchStatus);
    if(batchTerminationRequested)
    {
        delete sweeperGame;
        batchStatus->batchState = SweeperBatchStatus::TERMINATED;
        emit triggerUpdateOverview(batchStatus);
    }
}

void SweeperBatchManager::requestBatchTermination()
{
    batchTerminationRequested = true;
}
