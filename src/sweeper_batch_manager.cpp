#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    threadsInBatch = 0;
    batchStatus = new SweeperBatchStatus(this);
}

SweeperBatchManager::~SweeperBatchManager()
{
    killAllThreadsAndGames();
    delete batchSettings;
    delete batchStatus;
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

void SweeperBatchManager::launchBatch(SweeperBatchSettings* newBatchSettings)
{
    // Prevent attempting to launch multiple times
    if(batchStatus->batchState == SweeperBatchStatus::LOADING ||
       batchStatus->batchState == SweeperBatchStatus::IN_PROGRESS) return;
    batchStatus->batchState = SweeperBatchStatus::LOADING;

    // Save the batch settings that got passed in
    newBatchSettings->copyTo(batchSettings);

    // Reset the batch status
    batchStatus->gamesPlayed = 0;
    batchStatus->gamesWon = 0;
    batchStatus->totalGames = batchSettings->gameCount;
    emit triggerUpdateOverview(batchStatus);

    // Determine how many threads to use for the batch
    threadsInBatch = batchSettings->gameCount > batchSettings->maxThreadCount ? batchSettings->maxThreadCount :
                                                                                batchSettings->gameCount;

    // Start a new game group (collection of objects which correspond to a single game instance) for every thread.
    gameGroups = new GameGroup*[threadsInBatch];
    for(int i = 0; i < threadsInBatch; i++)
    {
        gameGroups[i] = new GameGroup();

        // Create game
        SweeperGame* sweeperGame  = new SweeperGame(i, batchSettings);
        gameGroups[i]->sweeperGame = sweeperGame;
        connect(sweeperGame, &SweeperGame::triggerSweeperGameIsAlive, [=]()
        {
            // Mark the game as fully started up
            gameGroups[i]->sweeperGameIsAlive = true;

            // Check if this was the last game we were waiting on before signaling batch launched to the control window
            emitIfBatchLaunched();
        });
        connect(sweeperGame, &SweeperGame::destroyed, [=]()
        {
            // Mark the game as fully shut down
            gameGroups[i]->sweeperGameIsAlive = false;

            // Check if this was the last game we were waiting on before signaling batch done to the control window
            emitIfBatchDone();
        });

        // Create thread
        QThread* thread = new QThread();
        gameGroups[i]->thread = thread;

        // All calls to the game object will be processed by the new thread
        sweeperGame->moveToThread(thread);
        connect(thread, SIGNAL(started()), sweeperGame, SLOT(doBeginGame()));

        // Create GUI (if selected by user)
        if(batchSettings->showGui)
        {
            // Unfortunately Qt only allows the main thread to control GUI objects, so the game object will have to
            // call us back when it's ready for the GUI to be shown or deleted.
            connect(sweeperGame, SIGNAL(triggerSpawnGui(int, SweeperModel*)),
                    this, SLOT(doSpawnGui(int, SweeperModel*)));
        }

        // Enable the game to let us know once it's over
        connect(sweeperGame, SIGNAL(triggerEndOfGame(int, SweeperModel::GAME_STATE)),
                this, SLOT(doEndOfGame(int, SweeperModel::GAME_STATE)));
        gameGroups[i]->acceptSignalsFromGame = true;

        // Start the game's thread
        gameGroups[i]->thread->start();
    }
}

void SweeperBatchManager::doSpawnGui(int index, SweeperModel* sweeperModel)
{
    // Generate widget
    SweeperWidget* sweeperWidget = new SweeperWidget(sweeperModel);
    gameGroups[index]->sweeperWidget = sweeperWidget;
    gameGroups[index]->sweeperWidgetIsAlive = true;
    connect(sweeperWidget, &SweeperWidget::destroyed, [=]()
    {
        gameGroups[index]->sweeperWidgetIsAlive = false;
        emitIfBatchDone();
    });

    // Connect GUI actions to their corresponding handlers in the game object
    SweeperGame* sweeperGame = gameGroups[index]->sweeperGame;
    connect(sweeperWidget, SIGNAL(triggerFlagAction(QPoint)), sweeperGame, SLOT(doFlagAction(QPoint)));
    connect(sweeperWidget, SIGNAL(triggerQuitAction()), sweeperGame, SLOT(doQuitAction()));
    connect(sweeperWidget, SIGNAL(triggerRevealAction(QPoint)), sweeperGame, SLOT(doRevealAction(QPoint)));
    connect(sweeperWidget, SIGNAL(triggerRevealAdjacentAction(QPoint)),
            sweeperGame, SLOT(doRevealAdjacentAction(QPoint)));

    // We don't know if this was the last required set of GUI elements for all the games in the batch since this method
    // was triggered asynchronously from another thread.  We'll check now.
    emitIfBatchLaunched();
}

void SweeperBatchManager::doTerminateBatch()
{
    // Make sure there's actually a running batch to terminate
    if(batchStatus->batchState != SweeperBatchStatus::IN_PROGRESS) return;

    killAllThreadsAndGames();
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::emitIfBatchLaunched()
{
    bool batchLaunched = true;
    for(int i = 0; i < threadsInBatch; i++)
    {
        if(!gameGroups[i]) batchLaunched = false;
        else if(!gameGroups[i]->sweeperGameIsAlive) batchLaunched = false;
        else if(batchSettings->showGui && !gameGroups[i]->sweeperWidgetIsAlive) batchLaunched = false;
    }
    if(batchLaunched)
    {
        batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
        if(batchSettings->showGui)
        {
            for(int i = 0; i < threadsInBatch; i++)
            {
                gameGroups[i]->sweeperWidget->show();
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
        if(!gameGroups[i]) batchDone = false;
        else if(gameGroups[i]->sweeperGameIsAlive) batchDone = false;
        else if(batchSettings->showGui && gameGroups[i]->sweeperWidgetIsAlive) batchDone = false;
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
            if(!gameGroups || !gameGroups[index] || !gameGroups[index]->acceptSignalsFromGame) return;
            if(batchSettings->showGui)
            {
                if(gameGroups[index]->sweeperWidget)
                {
                    delete gameGroups[index]->sweeperWidget;
                }
                if(gameGroups[index]->sweeperGame)
                {
                    gameGroups[index]->sweeperGame->safeToDeleteModel = true;
                }
            }
        }
        gameGroups[i]->acceptSignalsFromGame = false;
        gameGroups[i]->sweeperGame->deleteLater();
        gameGroups[i]->thread->quit();
        gameGroups[i]->thread->deleteLater();
    }
}
