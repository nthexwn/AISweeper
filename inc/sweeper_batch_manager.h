#ifndef SWEEPER_BATCH_MANAGER_H
#define SWEEPER_BATCH_MANAGER_H

#include <QFrame>
#include <QMutex>
#include <QObject>
#include <QVBoxLayout>
#include "sweeper_batch_status.h"
#include "sweeper_batch_settings.h"
#include "sweeper_game.h"
#include "sweeper_widget.h"

class SweeperBatchManager : public QObject
{
    Q_OBJECT
public:
    struct GameGroup
    {
        bool acceptSignalsFromGame;
        QPointer<SweeperGame> sweeperGame;
        bool sweeperGameIsAlive;
        QPointer<SweeperWidget> sweeperWidget;
        bool sweeperWidgetIsAlive;
        QPointer<QThread> thread;
        GameGroup()
        {
            acceptSignalsFromGame = false;
            sweeperGameIsAlive = false;
            sweeperWidgetIsAlive = false;
        }
    };
    explicit SweeperBatchManager(QObject* parent = 0);
    ~SweeperBatchManager();

signals:
    void triggerBatchDone();
    void triggerBatchLaunched();
    void triggerBeginGame();
    void triggerUpdateOverview(SweeperBatchStatus* batchStatus);

public slots:
    void doEndOfGame(int index, SweeperModel::GAME_STATE gameState);
    void doLaunchBatch(SweeperBatchSettings* batchSettings);
    void doSpawnGui(int index, SweeperModel* sweeperModel);
    void doTerminateBatch();

private:
    GameGroup** gameGroups;
    SweeperBatchSettings* batchSettings;
    SweeperBatchStatus* batchStatus;
    int threadsInBatch;
    void emitIfBatchLaunched();
    void emitIfBatchDone();
    void killAllThreadsAndGames();
};

#endif // SWEEPER_BATCH_MANAGER_H
