#ifndef SWEEPER_BATCH_MANAGER_H
#define SWEEPER_BATCH_MANAGER_H

#include <QObject>
#include "sweeper_batch_status.h"
#include "sweeper_batch_settings.h"
#include "sweeper_game.h"
#include "sweeper_widget.h"

class SweeperBatchManager : public QObject
{
    Q_OBJECT
public:
    explicit SweeperBatchManager(QObject* parent = 0);
    ~SweeperBatchManager();

signals:
    void triggerBatchDone();
    void triggerBatchLaunched();
    void triggerSetupGame();
    void triggerSetupPlayer();
    void triggerStartGame();
    void triggerTeardownGame();
    void triggerTeardownPlayer();
    void triggerDeleteSweeperWidget(SweeperWidget* sweeperWidget, int index);
    void triggerGenerateSweeperWidget(SweeperModel* sweeperModel, int index);
    void triggerShowSweeperWidget(SweeperWidget* sweeperWidget);
    void triggerUpdateOverview(SweeperBatchStatus* batchStatus);

public slots:
    void doSweeperWidgetDeleted(int index);
    void doSweeperWidgetGenerated(SweeperWidget* sweeperWidget, int index);
    void doGameDone(int index, SweeperModel::GAME_STATE gameState);
    void doGamePrepared(int index);
    void doPlayerDone(int index);
    void doPlayerPrepared(int index);
    void doLaunchBatch(SweeperBatchSettings* batchSettings);
    void doTerminateBatch();

private:
    enum ControlObjectState
    {
        INACTIVE,
        ACTIVATING,
        ACTIVE,
        DEACTIVATING
    };

    // A collection of objects used to launch and terminate every game.
    struct GameControlGroup
    {
        QPointer<SweeperGame> game;
        ControlObjectState gameControlState;
        SweeperWidget* widget;
        ControlObjectState widgetControlState;
        PlayerAbstract* player;
        ControlObjectState playerControlState;
        QPointer<QThread> thread;
        GameControlGroup()
        {
            gameReady = INACTIVE;
            widgetReady = INACTIVE;
            playerReady = INACTIVE;
        }
    };
    GameControlGroup** gameControlGroups;
    SweeperBatchSettings* batchSettings;
    SweeperBatchStatus* batchStatus;
    bool terminationRequested;
    int threadsInBatch;
    createObjectsForNewGame(int index);
    startGameIfReady(int index);
};

#endif // SWEEPER_BATCH_MANAGER_H
