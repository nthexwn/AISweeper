#include <QDebug>
#include "../inc/sweeper_batch_manager.h"

SweeperBatchManager::SweeperBatchManager(QObject* parent) : QObject(parent)
{
    qDebug() << "SweeperBatchManager::SweeperBatchManager.begin";
    threadsInBatch = 0;
    batchStatus = new SweeperBatchStatus(this);
    qDebug() << "SweeperBatchManager::SweeperBatchManager.end";
}

SweeperBatchManager::~SweeperBatchManager()
{
    killAllThreadsAndGames();
}

void SweeperBatchManager::launchBatch(SweeperBatchSettings* batchSettings)
{
    qDebug() << "SweeperBatchManager::launchBatch.begin";
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
        gameGroups[i]->sweeperGame = sweeperGame;
        sweeperGame->moveToThread(thread);
        if(batchSettings->showGui)
        {
            connect(sweeperGame, SIGNAL(triggerKillGui(int)), this, SLOT(doKillGui(int)));
            connect(sweeperGame, SIGNAL(triggerSpawnGui(int, SweeperModel*)),
                    this, SLOT(doSpawnGui(int, SweeperModel*)));
        }
        connect(sweeperGame, SIGNAL(triggerEndOfGame(int, SweeperModel::GAME_STATE)),
                this, SLOT(doEndOfGame(int, SweeperModel::GAME_STATE)));
        connect(thread, SIGNAL(started()),sweeperGame, SLOT(doBeginGame()));
        thread->start();
    }
    batchStatus->batchState = SweeperBatchStatus::IN_PROGRESS;
    batchStatus->gamesPlayed = 0;
    batchStatus->gamesWon = 0;
    batchStatus->totalGames = batchSettings->gameCount;
    emit triggerUpdateOverview(batchStatus);
    qDebug() << "SweeperBatchManager::launchBatch.end";
}

void SweeperBatchManager::terminateBatch()
{
    qDebug() << "SweeperBatchManager::terminateBatch.begin";
    if(batchStatus->batchState != SweeperBatchStatus::IN_PROGRESS) return;
    killAllThreadsAndGames();
    batchStatus->batchState = SweeperBatchStatus::TERMINATED;
    emit triggerUpdateOverview(batchStatus);
    qDebug() << "SweeperBatchManager::terminateBatch.end";
}

void SweeperBatchManager::doEndOfGame(int index, SweeperModel::GAME_STATE gameState)
{
    qDebug() << "SweeperBatchManager::doEndOfGame.begin";
    batchStatus->gamesPlayed++;
    if(gameState == SweeperModel::WON) batchStatus->gamesWon++;
    if(batchStatus->totalGames - batchStatus->gamesPlayed > 0)
    {
        SweeperGame* lastSweeperGame = gameGroups[index]->sweeperGame;
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
        connect(sweeperGame, SIGNAL(triggerEndOfGame(int, SweeperModel::GAME_STATE)),
                this, SLOT(doEndOfGame(int, SweeperModel::GAME_STATE)));
        disconnect(this, SIGNAL(triggerBeginGame()));
        connect(this, SIGNAL(triggerBeginGame()), sweeperGame, SLOT(doBeginGame()));
        emit triggerBeginGame();
    }
    else
    {
        killAllThreadsAndGames();
        batchStatus->batchState = SweeperBatchStatus::COMPLETED;
    }
    emit triggerUpdateOverview(batchStatus);
    qDebug() << "SweeperBatchManager::doEndOfGame.end";
}

void SweeperBatchManager::doKillGui(int index)
{
    qDebug() << "SweeperBatchManager::doKillGui.begin";
    if(gameGroups[index]->sweeperWidget) gameGroups[index]->sweeperWidget->deleteLater();
    if(gameGroups[index]->layout) gameGroups[index]->layout->deleteLater();
    if(gameGroups[index]->frame) gameGroups[index]->frame->deleteLater();
    qDebug() << "SweeperBatchManager::doKillGui.end";
}

void SweeperBatchManager::doSpawnGui(int index, SweeperModel* sweeperModel)
{
    qDebug() << "SweeperBatchManager::doSpawnGui.begin";
    // Game already exists
    SweeperGame* sweeperGame = gameGroups[index]->sweeperGame;

    // Generate widget
    SweeperWidget* sweeperWidget = new SweeperWidget(sweeperModel);
    gameGroups[index]->sweeperWidget = sweeperWidget;

    // Generate frame
    QFrame* frame = new QFrame();
    gameGroups[index]->frame = frame;
    frame->setFrameShape(QFrame::Panel);

    // Generate layout
    QVBoxLayout* layout = new QVBoxLayout();
    gameGroups[index]->layout = layout;
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
    qDebug() << "SweeperBatchManager::doSpawnGui.end";
}

void SweeperBatchManager::killAllThreadsAndGames()
{
    qDebug() << "SweeperBatchManager::killAllThreadsAndGames.begin";
    if(batchStatus->batchState != SweeperBatchStatus::IN_PROGRESS) return;
    for(int i = 0; i < threadsInBatch; i++)
    {
        if(batchSettings->showGui)
        {
            doKillGui(i);
        }
        gameGroups[i]->sweeperGame->deleteLater();
        gameGroups[i]->thread->quit();
        gameGroups[i]->thread->deleteLater();
        delete gameGroups[i];
    }
    delete gameGroups;
    qDebug() << "SweeperBatchManager::killAllThreadsAndGames.end";
}
