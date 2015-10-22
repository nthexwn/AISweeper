#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    batchLaunchRequested = false;
    batchStatus = new SweeperBatchStatus();
    batchTerminationRequested = false;
}

SweeperBatchManager::~SweeperBatchManager()
{
    delete batchStatus;
    batchStatus = nullptr;
    delete sweeperGame;
    sweeperGame = nullptr;
}

void SweeperBatchManager::launchBatch(SweeperBatchSettings* batchSettings)
{
    if(batchLaunchRequested) return;
    batchLaunchRequested = true;
    sweeperGame = new SweeperGame(batchSettings);
    batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
    emit triggerUpdateOverview(batchStatus);
    if(batchTerminationRequested)
    {
        delete sweeperGame;
        sweeperGame = nullptr;
        batchStatus->batchState = SweeperBatchStatus::TERMINATED;
        emit triggerUpdateOverview(batchStatus);
    }
    batchLaunchRequested = false;
}

void SweeperBatchManager::requestBatchTermination()
{
    batchTerminationRequested = true;
}
