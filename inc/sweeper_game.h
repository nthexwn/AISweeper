#ifndef SWEEPER_GAME_H
#define SWEEPER_GAME_H

#include "./players/player_abstract.h"
#include "./players/player_human.h"
#include "./players/player_machine_learning_ai.h"
#include "./players/player_probability_based_ai.h"
#include "./players/player_random_action_ai.h"
#include "sweeper_batch_settings.h"
#include "sweeper_model.h"
#include "sweeper_widget.h"

class SweeperGame : public QObject
{
    Q_OBJECT
public:
    SweeperGame(int index, SweeperBatchSettings* batchSettings, QObject* parent = 0);
    ~SweeperGame();

signals:
    void triggerGameHasEnded(int index, SweeperModel::GAME_STATE gameState);
    void triggerSweeperGameIsAlive();
    void triggerTakeNextAction();

public slots:
    void doBeginGame();
    void doDelete();
    void doFlagAction(QPoint nodeIndex);
    void doQuitAction();
    void doRevealAction(QPoint nodeIndex);
    void doRevealAdjacentAction(QPoint nodeIndex);

private:
    SweeperBatchSettings* batchSettings;
    bool firstReveal;
    int index;
    PlayerAbstract* player;
    SweeperModel* sweeperModel;
    SweeperWidget* sweeperWidget;
    int countAdjacentFlagsHelper(SweeperNode* node);
    SweeperNode::NODE_STATE countAdjacentMines(SweeperNode* node);
    int countAdjacentMinesHelper(SweeperNode* node);
    void handlePauseAndTakeNextAction();
    void handleWinMechanics();
    void revealAllMines();
    void revealNode(SweeperNode* node);
};

#endif // SWEEPER_GAME_H
