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
        QFrame* frame;
        QVBoxLayout* layout;
        SweeperGame* sweeperGame;
        SweeperWidget* sweeperWidget;
        QThread* thread;
    };
    explicit SweeperBatchManager(QObject* parent = 0);
    ~SweeperBatchManager();
    void launchBatch(SweeperBatchSettings* batchSettings);
    void terminateBatch();

signals:
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
    void killAllThreadsAndGames();
};

#endif // SWEEPER_BATCH_MANAGER_H
