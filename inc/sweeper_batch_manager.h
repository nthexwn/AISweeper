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
        QPointer<QFrame> frame;
        bool frameIsAlive;
        QPointer<QVBoxLayout> layout;
        bool layoutIsAlive;
        QPointer<SweeperGame> sweeperGame;
        bool sweeperGameIsAlive;
        QPointer<SweeperWidget> sweeperWidget;
        bool sweeperWidgetIsAlive;
        QPointer<QThread> thread;
        bool threadIsAlive;
        GameGroup()
        {
            acceptSignalsFromGame = false;
            frameIsAlive = false;
            layoutIsAlive = false;
            sweeperGameIsAlive = false;
            sweeperWidgetIsAlive = false;
            threadIsAlive = false;
        }
    };
    explicit SweeperBatchManager(QObject* parent = 0);
    ~SweeperBatchManager();
    void launchBatch(SweeperBatchSettings* batchSettings);
    void terminateBatch();

signals:
    void triggerBatchDone();
    void triggerBatchLaunched();
    void triggerBeginGame();
    void triggerUpdateOverview(SweeperBatchStatus* batchStatus);

public slots:
    void doEndOfGame(int index, SweeperModel::GAME_STATE gameState);
    void doKillGui(int index);
    void doSpawnGui(int index, SweeperModel* sweeperModel);

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
