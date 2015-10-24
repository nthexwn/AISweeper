#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    threadsInBatch = 0;
    batchStatus = new SweeperBatchStatus();
}

SweeperBatchManager::~SweeperBatchManager()
{
    for(int i = 0; i < threadsInBatch; i++) threads[i]->deleteLater();
    delete threads;
    delete batchStatus;
    batchStatus = nullptr;
}

void SweeperBatchManager::doEndGame(SweeperGame* sweeperGame)
{
    batchStatus->gamesPlayed++;
    if(sweeperGame->sweeperModel->gameState == SweeperModel::WON) batchStatus->gamesWon++;
    if(batchStatus->totalGames - batchStatus->gamesPlayed > 0)
    {
        QThread* thread = sweeperGame->thread();
        delete sweeperGame;
        SweeperGame* sweeperGame = new SweeperGame(batchSettings);
        sweeperGame->moveToThread(thread);
        connect(sweeperGame, SIGNAL(triggerEndGame(SweeperGame*)), this, SLOT(doEndGame(SweeperGame*)));
        disconnect(this, SIGNAL(triggerBeginGame()));
        connect(this, SIGNAL(triggerBeginGame()), sweeperGame, SLOT(doBeginGame()));
        emit triggerBeginGame();
    }
    else
    {
        batchStatus->batchState = SweeperBatchStatus::COMPLETED;
        for(int i = 0; i < threadsInBatch; i++) threads[i]->deleteLater();
        delete threads;
    }
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::launchBatch(SweeperBatchSettings* batchSettings)
{
    if(batchStatus->batchState == SweeperBatchStatus::IN_PROGRESS) return;
    this->batchSettings = batchSettings;
    threadsInBatch = batchSettings->gameCount > batchSettings->maxThreadCount ? batchSettings->maxThreadCount :
                                                                                batchSettings->gameCount;
    threads = new QThread*[threadsInBatch];
    for(int i = 0; i < threadsInBatch; i++)
    {
        threads[i] = new QThread();
        SweeperGame* sweeperGame = new SweeperGame(batchSettings);
        sweeperGame->moveToThread(threads[i]);
        connect(sweeperGame, SIGNAL(triggerEndGame(SweeperGame*)), this, SLOT(doEndGame(SweeperGame*)));
        connect(threads[i], SIGNAL(started()), sweeperGame, SLOT(doBeginGame()));
        threads[i]->start();
    }
    batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
    batchStatus->gamesPlayed = 0;
    batchStatus->gamesWon = 0;
    batchStatus->totalGames = batchSettings->gameCount;
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::terminateBatch()
{
    if(batchStatus->batchState != SweeperBatchStatus::IN_PROGRESS) return;
    for(int i = 0; i < threadsInBatch; i++) threads[i]->deleteLater();
    delete threads;
    batchStatus->batchState = SweeperBatchStatus::TERMINATED;
    emit triggerUpdateOverview(batchStatus);
}
