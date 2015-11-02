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
    }
    for(int i = 0; i < threadsInBatch; i++)
    {
        gameGroups[i]->thread->start();
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

        // We can't call the doBeginGame method of the new game object directly since it's running on another thread.
        // Instead, we'll emit a trigger signal.  We only want to start one new game, so we'll disconnect the trigger
        // signal from all other game objects before connecting it and calling it to start the new game.
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
            delete gameGroups[index]->frame;
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
    gameGroups[index]->sweeperWidgetIsAlive = true;

    // Generate frame
    QFrame* frame = new QFrame();
    gameGroups[index]->frame = frame;
    connect(frame, &QFrame::destroyed, [=]()
    {
        gameGroups[index]->frameIsAlive = false;
        emitIfBatchDone();
    });
    frame->setFrameShape(QFrame::Panel);
    gameGroups[index]->frameIsAlive = true;

    // Generate layout
    QVBoxLayout* layout = new QVBoxLayout();
    gameGroups[index]->layout = layout;
    connect(layout, &QVBoxLayout::destroyed, [=]()
    {
        gameGroups[index]->layoutIsAlive = false;
        emitIfBatchDone();
    });
    layout->setMargin(0);
    gameGroups[index]->layoutIsAlive = true;

    // Attach the GUI objects that we just generated
    layout->addWidget(sweeperWidget);
    frame->setLayout(layout);

    // Connect GUI actions to their corresponding handlers in the game object
    connect(sweeperWidget, SIGNAL(triggerFlagAction(QPoint)), sweeperGame, SLOT(doFlagAction(QPoint)));
    connect(sweeperWidget, SIGNAL(triggerQuitAction()), sweeperGame, SLOT(doQuitAction()));
    connect(sweeperWidget, SIGNAL(triggerRevealAction(QPoint)), sweeperGame, SLOT(doRevealAction(QPoint)));
    connect(sweeperWidget, SIGNAL(triggerRevealAdjacentAction(QPoint)),
            sweeperGame, SLOT(doRevealAdjacentAction(QPoint)));

    // We don't know if this was the last required set of GUI elements for all the games in the batch since this method
    // was triggered asynchronously from another thread.  We'll check now.
    emitIfBatchLaunched();
}

void SweeperBatchManager::emitIfBatchLaunched()
{
    if(!gameGroups[threadsInBatch - 1]) return;
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
        if(batchSettings->showGui)
        {
            for(int i = 0; i < threadsInBatch; i++)
            {
                gameGroups[i]->frame->show();
            }
        }
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
