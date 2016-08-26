#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    terminationRequested = false;
    threadsInBatch = 0;
    batchStatus = new SweeperBatchStatus(this);
    batchSettings = new SweeperBatchSettings(this);
}

void SweeperBatchManager::doLaunchBatch(SweeperBatchSettings* newBatchSettings)
{
    // In these cases we don't want to launch a batch so we return immediately
    if(terminationRequested ||
       batchStatus->batchState == SweeperBatchStatus::LOADING ||
       batchStatus->batchState == SweeperBatchStatus::IN_PROGRESS ||
       batchStatus->batchState == SweeperBatchStatus::TERMINATING) return;

    // Mark batch as loading
    batchStatus->batchState = SweeperBatchStatus::LOADING;

    // Save the batch settings that got passed in
    newBatchSettings->copyTo(batchSettings);

    // Reset the batch stats
    batchStatus->gamesPlayed = 0;
    batchStatus->gamesWon = 0;
    batchStatus->gamesUnfinished = 0;
    batchStatus->totalGames = batchSettings->gameCount;
    emit triggerUpdateOverview(batchStatus);

    // Determine how many threads to use for the batch
    threadsInBatch = batchSettings->gameCount > batchSettings->maxThreadCount ? batchSettings->maxThreadCount :
                                                                                batchSettings->gameCount;

    // Start a new game control group (collection of objects which correspond to a single game instance) for every
    // thread.
    gameControlGroups = new GameControlGroup*[threadsInBatch];
    for(int index = 0; index < threadsInBatch; index++)
    {
        // Create group
        gameControlGroups[index] = new GameControlGroup();

        // Create thread
        QThread* thread = new QThread();
        gameControlGroups[index]->thread = thread;

        // Start thread
        thread->start();

        // Create the game and player objects
        createObjectsForNewGame(index);
    }

    // Mark batch as in progress and change 'launch' button to 'terminate'
    batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
    emit triggerUpdateOverview(batchStatus);
    emit triggerBatchLaunched();
}

// Called either when creating the first game for each thread or when starting a new game on the
// same thread after the last one finished.
void SweeperBatchManager::createObjectsForNewGame(int index)
{
    // Create game
    SweeperGame* game  = new SweeperGame(index, batchSettings);
    gameControlGroups[index]->game = game;

    // Move game to assigned thread
    QThread* thread = gameControlGroups[index]->thread;
    game->moveToThread(thread);

    // Connect game signals and slots
    connect(game, SIGNAL(triggerGamePrepared(int)), this, SLOT(doGamePrepared(int)));
    connect(game, SIGNAL(triggerGameDone(int)), this, SLOT(doGameDone(int)));
    connect(this, SIGNAL(triggerKillGames()), game, SLOT(doQuitAction()));

    // Tell game to activate
    gameControlGroups[index]->gameControlState = SweeperBatchManager::ACTIVATING;
    connect(this, SIGNAL(triggerPrepareGame()), game, SLOT(doPrepareGame()));
    emit triggerSetupGame();
    disconnect(this, SIGNAL(triggerPrepareGame), game, SLOT(doPrepareGame()));

    // Create player
    PlayerAbstract* player;
    switch(batchSettings->playerType)
    {
    case SweeperBatchSettings::HUMAN:
        player = new PlayerHuman(game->sweeperModel);
        break;
    case SweeperBatchSettings::RANDOM_ACTION_AI:
        player = new PlayerRandomActionAi(game->sweeperModel);
        break;
    case SweeperBatchSettings::PROBABILITY_BASED_AI:
        player = new PlayerProbabilityBasedAi(game->sweeperModel);
        break;
    case SweeperBatchSettings::MACHINE_LEARNING_AI:
        player = new PlayerMachineLearningAi(game->sweeperModel);
        break;
    }
    gameControlGroups[index]->player = player;

    // Move player to assigned thread
    player->moveToThread(thread);

    // Connect player signals and slots
    connect(player, SIGNAL(triggerPlayerDone(int)), this, SLOT(doPlayerDone(int)));
    connect(player, SIGNAL(triggerPlayerPrepared(int)), this, SLOT(doPlayerPrepared(int)));
    connect(this, SIGNAL(triggerKillPlayers()), player, SLOT(doQuitPlayer()));

    // Tell player to activate
    gameControlGroups[index]->playerControlState = SweeperBatchManager::ACTIVATING;
    connect(this, SIGNAL(triggerSetupPlayer()), player, SLOT(doSetupPlayer()));
    emit triggerSetupPlayer();
    disconnect(this, SIGNAL(triggerSetupPlayer), player, SLOT(doSetupPlayer()));
}

void SweeperBatchManager::doGamePrepared(int index)
{
    gameControlGroups[index]->gameControlState = ACTIVE;
    if(terminationRequested)
    {
        gameControlGroups[index]->gameControlState = DEACTIVATING;
        emit triggerKillGames();
    }
    else if(batchSettings->showGui == true)
    {
        SweeperModel* model = gameControlGroups[index]->game->sweeperModel;
        emit triggerGenerateSweeperWidget(model, index);
    }
    else
    {
        startGameIfReady(index);
    }
}

void SweeperBatchManager::doPlayerPrepared(int index)
{
    gameControlGroups[index]->playerControlState = ACTIVE;
    if(terminationRequested)
    {
        gameControlGroups[index]->playerControlState = DEACTIVATING;
        emit triggerKillPlayers();
    }
    else
    {
        startGameIfReady(index);
    }
}

void SweeperBatchManager::doSweeperWidgetGenerated(SweeperWidget* widget, int index)
{
    gameControlGroups[index]->widget = widget;
    gameControlGroups[index]->widgetControlState = ACTIVE;
    if(terminationRequested)
    {
        gameControlGroups[index]->widgetControlState = DEACTIVATING;
        emit triggerDeleteSweeperWidget(widget, index);
    }
    else
    {
        startGameIfReady(index);
    }
}

void SweeperBatchManager::startGameIfReady(int index)
{
    if(batchSettings->showGui == true && gameControlGroups[index]->widgetControlState == ControlObjectState::ACTIVE)
    {
        // We still have to wait for the widget to be generated for this game before we can start it.
        return;
    }
    if(gameControlGroups[index]->gameControlState == ControlObjectState::ACTIVE && gameControlGroups[index]->playerControlState == ControlObjectState::ACTIVE)
    {
        // Tell the game to start
        connect(this, SIGNAL(triggerStartPlayer()), gameControlGroups[index]->player, SLOT(doTakeNextAction()));
        emit triggerStartPlayer();
        disconnect(this, SIGNAL(triggerStartPlayer()));
    }
}

void SweeperBatchManager::doGameDone(int index, SweeperModel::GAME_STATE gameState)
{
    if(!gameControlGroups || !gameControlGroups[index] || !gameControlGroups[index]->game) return;

    // Track game as a win, loss, or unfinished (IE: User closed the game's window)
    batchStatus->gamesPlayed++;
    if(gameState == SweeperModel::WON) batchStatus->gamesWon++;
    else if(gameState == SweeperModel::UNFINISHED) batchStatus->gamesUnfinished++;

    // Delete the game that just ended.
    SweeperGame* lastSweeperGame = gameControlGroups[index]->game;
    delete lastSweeperGame;

    // If there are more games to launch in this batch then we'll launch another one on the same thread as the game
    // that just ended.
    if(batchStatus->totalGames - batchStatus->gamesInProgress - batchStatus->gamesPlayed -
            batchStatus->gamesUnfinished > 0)
    {
        SweeperGame* sweeperGame = new SweeperGame(index, batchSettings);
        gameControlGroups[index]->game = sweeperGame;
        sweeperGame->moveToThread(gameControlGroups[index]->thread);
        connect(sweeperGame, SIGNAL(triggerGameHasEnded(int, SweeperModel::GAME_STATE)),
                this, SLOT(doEndOfGame(int, SweeperModel::GAME_STATE)));

        // We can't call the doBeginGame method of the new game object directly since it's running on another thread.
        // Instead, we'll emit a trigger signal.  We only want to start one new game, so we'll disconnect the trigger
        // signal from all other game objects before connecting it and calling it to start the new game.
        disconnect(this, SIGNAL(triggerSetupGame()));
        connect(this, SIGNAL(triggerStartGame()), sweeperGame, SLOT(doBeginGame()));
        emit triggerStartPlayer();
    }

    // Update the overview regardless of how game ended.
    SweeperBatchStatus* newBatchStatus = new SweeperBatchStatus();
    batchStatus->copyTo(newBatchStatus);
    emit triggerUpdateOverview(newBatchStatus);
}

// Called when user presses the "terminate" button
void SweeperBatchManager::doTerminateBatch()
{
    // This flag will prevent new games from being launched
    terminationRequested = true;

    // We don't want to send the quit signals if the game objects aren't running or are already terminating.
    if(batchStatus->batchState == SweeperBatchStatus::IN_PROGRESS)
    {
        for(int i = 0; i < threadsInBatch; i++)
        {
            switch(gameControlGroups[i]->gameControlState)
            {
            case SweeperBatchManager::INACTIVE:
                // Nothing to do
                break;
            case SweeperBatchManager::ACTIVATING:
                // We'll send a quit signal after we receive the gamePrepared signal
                break;
            case SweeperBatchManager::ACTIVE:
                // The game is running and needs to be stopped, so we'll send the quitGame signal
                gameControlGroups[i]->gameControlState = SweeperBatchManager::DEACTIVATING;
                connect(this, SIGNAL(triggerQuitGame()), gameControlGroups[i]->game, SLOT(doQuitGame()));
                emit triggerKillGames();
                disconnect(this, SIGNAL(triggerQuitGame()), gameControlGroups[i]->game, SLOT(doQuitGame()));
                break;
            case SweeperBatchManager::DEACTIVATING:
                // The game is already deactivating so we'll continue waiting for the gameDone signal
                break;
            }

            switch(gameControlGroups[i]->playerControlState)
            {
            case SweeperBatchManager::INACTIVE:
                // Nothing to do
                break;
            case SweeperBatchManager::ACTIVATING:
                // We'll send a quit signal after we receive the playerPrepared signal
                break;
            case SweeperBatchManager::ACTIVE:
                // The player is running and needs to be stopped, so we'll send the quitPlayer signal
                gameControlGroups[i]->playerControlState = SweeperBatchManager::DEACTIVATING;
                connect(this, SIGNAL(triggerQuitPlayer()), gameControlGroups[i]->player, SLOT(doQuitPlayer()));
                emit triggerKillPlayers();
                disconnect(this, SIGNAL(triggerQuitPlayer()), gameControlGroups[i]->player, SLOT(doQuitPlayer()));
                break;
            case SweeperBatchManager::DEACTIVATING:
                // The player is already deactivating so we'll continue waiting for the playerDone signal
                break;
            }

            switch(gameControlGroups[i]->widgetControlState)
            {
            case SweeperBatchManager::INACTIVE:
                // Nothing to do
                break;
            case SweeperBatchManager::ACTIVATING:
                // Blah
                break;
            case SweeperBatchManager::ACTIVE:
                // Blah
                break;
            case SweeperBatchManager::DEACTIVATING:
                // Blah
                break;
            }
        }
    }

    // Mark batch as terminating and show in control window
    batchStatus->batchState = SweeperBatchStatus::TERMINATING;
    emit triggerUpdateOverview(batchStatus);
}

void SweeperBatchManager::doSweeperWidgetDeleted(int index)
{
    // TODO
}

void SweeperBatchManager::doPlayerDone(int index)
{
    // TODO
}

SweeperBatchManager::~SweeperBatchManager()
{
    // TODO
}
