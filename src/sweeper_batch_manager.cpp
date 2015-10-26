#include <QDebug>
#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    threadsInBatch = 0;
    batchStatus = new SweeperBatchStatus(this);
}

SweeperBatchManager::~SweeperBatchManager()
{
    killAllThreadsAndGames();
}

void SweeperBatchManager::launchBatch(SweeperBatchSettings* batchSettings)
{
    if(batchStatus->batchState == SweeperBatchStatus::IN_PROGRESS) return;
    this->batchSettings = batchSettings;
    threadsInBatch = batchSettings->gameCount > batchSettings->maxThreadCount ? batchSettings->maxThreadCount :
                                                                                batchSettings->gameCount;
    gameGroups = new GameGroup*[threadsInBatch];
    for(int i = 0; i < threadsInBatch; i++)
    {
        QThread* thread = new QThread();
        SweeperGame* sweeperGame  = new SweeperGame(i, batchSettings);
        gameGroups[i] = new GameGroup();
        gameGroups[i]->thread = thread;
        connect(thread, &QThread::destroyed, [=]()
        {
            gameGroups[i]->threadIsAlive = false;
            emitIfBatchDone();
        });
        connect(thread, &QThread::started, [=]()
        {
            gameGroups[i]->threadIsAlive = true;
            emitIfBatchLaunched();
        });
        gameGroups[i]->sweeperGame = sweeperGame;
        sweeperGame->moveToThread(thread);
        if(batchSettings->showGui)
        {
            connect(sweeperGame, SIGNAL(triggerKillGui(int)), this, SLOT(doKillGui(int)));
            connect(sweeperGame, SIGNAL(triggerSpawnGui(int, SweeperModel*)),
                    this, SLOT(doSpawnGui(int, SweeperModel*)));
        }
        connect(sweeperGame, &SweeperGame::destroyed, [=]()
        {
            gameGroups[i]->sweeperGameIsAlive = false;
            emitIfBatchDone();
        });
        connect(sweeperGame, &SweeperGame::triggerSweeperGameIsAlive, [=]()
        {
            gameGroups[i]->sweeperGameIsAlive = true;
            emitIfBatchLaunched();
        });
        connect(sweeperGame, SIGNAL(triggerEndOfGame(int, SweeperModel::GAME_STATE)),
                this, SLOT(doEndOfGame(int, SweeperModel::GAME_STATE)));
        connect(thread, SIGNAL(started()),sweeperGame, SLOT(doBeginGame()));
        gameGroups[i]->acceptSignalsFromGame = true;
        thread->start();
    }
    batchStatus->gamesPlayed = 0;
    batchStatus->gamesWon = 0;
    batchStatus->totalGames = batchSettings->gameCount;
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::terminateBatch()
{
    if(batchStatus->batchState != SweeperBatchStatus::IN_PROGRESS) return;
    killAllThreadsAndGames();
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::doEndOfGame(int index, SweeperModel::GAME_STATE gameState)
{
    if(!gameGroups || !gameGroups[index] || !gameGroups[index]->acceptSignalsFromGame) return;
    batchStatus->gamesPlayed++;
    if(gameState == SweeperModel::WON) batchStatus->gamesWon++;
    if(batchStatus->totalGames - batchStatus->gamesPlayed - threadsInBatch + 1 > 0)
    {
        SweeperGame* lastSweeperGame = gameGroups[index]->sweeperGame;
        gameGroups[index]->acceptSignalsFromGame = false;
        lastSweeperGame->deleteLater();
        SweeperGame* sweeperGame = new SweeperGame(index, batchSettings);
        gameGroups[index]->sweeperGame = sweeperGame;
        sweeperGame->moveToThread(gameGroups[index]->thread);
        if(batchSettings->showGui)
        {
            connect(sweeperGame, SIGNAL(triggerKillGui(int)), this, SLOT(doKillGui(int, SweeperModel*)));
            connect(sweeperGame, SIGNAL(triggerSpawnGui(int, SweeperModel*)),
                    this, SLOT(doSpawnGui(int, SweeperModel*)));
        }
        connect(sweeperGame, &SweeperGame::destroyed, [=]()
        {
            gameGroups[index]->sweeperGameIsAlive = false;
        });
        connect(sweeperGame, &SweeperGame::triggerSweeperGameIsAlive, [=]()
        {
            gameGroups[index]->sweeperGameIsAlive = true;
        });
        connect(sweeperGame, SIGNAL(triggerEndOfGame(int, SweeperModel::GAME_STATE)),
                this, SLOT(doEndOfGame(int, SweeperModel::GAME_STATE)));
        disconnect(this, SIGNAL(triggerBeginGame()));
        connect(this, SIGNAL(triggerBeginGame()), sweeperGame, SLOT(doBeginGame()));
        gameGroups[index]->acceptSignalsFromGame = true;
        emit triggerBeginGame();
    }
    else
    {
        killAllThreadsAndGames();
        batchStatus->batchState = SweeperBatchStatus::COMPLETED;
    }
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::doKillGui(int index)
{
    if(!gameGroups || !gameGroups[index] || !gameGroups[index]->acceptSignalsFromGame) return;
    if(batchSettings->showGui)
    {
        if(gameGroups[index]->frame)
        {
            SweeperGame* sweeperGame = gameGroups[index]->sweeperGame;
            connect(gameGroups[index]->sweeperWidget, &QObject::destroyed, [=]()
            {
                sweeperGame->safeToDeleteModel = true;
            });
            gameGroups[index]->frame->deleteLater();
        }
        else
        {
            gameGroups[index]->sweeperGame->safeToDeleteModel = true;
        }
    }
}

void SweeperBatchManager::doSpawnGui(int index, SweeperModel* sweeperModel)
{
    if(!gameGroups || !gameGroups[index] || !gameGroups[index]->acceptSignalsFromGame) return;

    // Game already exists
    SweeperGame* sweeperGame = gameGroups[index]->sweeperGame;

    // Generate widget
    SweeperWidget* sweeperWidget = new SweeperWidget(sweeperModel);
    gameGroups[index]->sweeperWidget = sweeperWidget;
    connect(sweeperWidget, &SweeperWidget::destroyed, [=]()
    {
        gameGroups[index]->sweeperWidgetIsAlive = false;
        emitIfBatchDone();
    });

    // Generate frame
    QFrame* frame = new QFrame();
    gameGroups[index]->frame = frame;
    connect(frame, &QFrame::destroyed, [=]()
    {
        gameGroups[index]->frameIsAlive = false;
        emitIfBatchDone();
    });
    frame->setFrameShape(QFrame::Panel);

    // Generate layout
    QVBoxLayout* layout = new QVBoxLayout();
    gameGroups[index]->layout = layout;
    connect(layout, &QVBoxLayout::destroyed, [=]()
    {
        gameGroups[index]->layoutIsAlive = false;
        emitIfBatchDone();
    });
    layout->setMargin(0);

    // Attach the GUI objects that we just generated
    layout->addWidget(sweeperWidget);
    frame->setLayout(layout);

    // Connect GUI actions to their corresponding handlers in the game object
    connect(sweeperWidget, SIGNAL(triggerFlagAction(QPoint)), sweeperGame, SLOT(doFlagAction(QPoint)));
    connect(sweeperWidget, SIGNAL(triggerQuitAction()), sweeperGame, SLOT(doQuitAction()));
    connect(sweeperWidget, SIGNAL(triggerRevealAction(QPoint)), sweeperGame, SLOT(doRevealAction(QPoint)));
    connect(sweeperWidget, SIGNAL(triggerRevealAdjacentAction(QPoint)),
            sweeperGame, SLOT(doRevealAdjacentAction(QPoint)));

    // Render the GUI on the screen
    frame->show();
    gameGroups[index]->frameIsAlive = true;
    gameGroups[index]->layoutIsAlive = true;
    gameGroups[index]->sweeperWidgetIsAlive = true;
    emitIfBatchLaunched();
}

void SweeperBatchManager::emitIfBatchLaunched()
{
    if(batchStatus->batchState == SweeperBatchStatus::IN_PROGRESS) return;
    bool batchLaunched = true;
    for(int i = 0; i < threadsInBatch; i++)
    {
        if(!gameGroups[i]) batchLaunched = false;
        else if(!gameGroups[i]->sweeperGameIsAlive) batchLaunched = false;
        else if(!gameGroups[i]->threadIsAlive) batchLaunched = false;
        else if(batchSettings->showGui)
        {
            if(!gameGroups[i]->frameIsAlive) batchLaunched = false;
            else if(!gameGroups[i]->layoutIsAlive) batchLaunched = false;
            else if(!gameGroups[i]->sweeperWidgetIsAlive) batchLaunched = false;
        }
    }
    if(batchLaunched)
    {
        batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
        emit triggerUpdateOverview(batchStatus);
        emit triggerBatchLaunched();
    }
}

void SweeperBatchManager::emitIfBatchDone()
{
    if(batchStatus->batchState == SweeperBatchStatus::TERMINATED) return;
    bool batchDone = true;
    for(int i = 0; i < threadsInBatch; i++)
    {
        if(gameGroups[i]->sweeperGameIsAlive) batchDone = false;
        else if(gameGroups[i]->threadIsAlive) batchDone = false;
        else if(batchSettings->showGui)
        {
            if(gameGroups[i]->frameIsAlive) batchDone = false;
            else if(gameGroups[i]->layoutIsAlive) batchDone = false;
            else if(gameGroups[i]->sweeperWidgetIsAlive) batchDone = false;
        }
    }
    if(batchDone)
    {
        for(int i = 0; i < threadsInBatch; i++) { delete gameGroups[i]; }
        delete gameGroups;
        if(batchStatus->batchState != SweeperBatchStatus::COMPLETED)
        {
            batchStatus->batchState = SweeperBatchStatus::TERMINATED;
        }
        emit triggerUpdateOverview(batchStatus);
        emit triggerBatchDone();
    }
}

void SweeperBatchManager::killAllThreadsAndGames()
{
    if(batchStatus->batchState != SweeperBatchStatus::IN_PROGRESS) return;
    for(int i = 0; i < threadsInBatch; i++)
    {
        if(batchSettings->showGui)
        {
            doKillGui(i);
        }
        gameGroups[i]->acceptSignalsFromGame = false;
        gameGroups[i]->sweeperGame->deleteLater();
        gameGroups[i]->thread->quit();
        gameGroups[i]->thread->deleteLater();
    }
}
