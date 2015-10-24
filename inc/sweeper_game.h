#ifndef SWEEPER_GAME_H
#define SWEEPER_GAME_H

#include <QFrame>
#include <QVBoxLayout>
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
    SweeperModel* sweeperModel;
    SweeperGame(SweeperBatchSettings* batchSettings, QObject* parent = 0);
    ~SweeperGame();

signals:
    void triggerEndGame(SweeperGame* thisGame);
    void triggerTakeNextAction();

public slots:
    void doBeginGame();
    void doFlagAction(QPoint nodeIndex);
    void doQuitAction();
    void doRevealAction(QPoint nodeIndex);
    void doRevealAdjacentAction(QPoint nodeIndex);

private:
    bool firstReveal;
    SweeperBatchSettings* batchSettings;
    PlayerAbstract* player;
    QFrame* frame;
    QVBoxLayout* layout;
    SweeperWidget* sweeperWidget;
    SweeperNode::NODE_STATE countAdjacentMines(SweeperNode* node);
    int countAdjacentFlagsHelper(SweeperNode* node);
    int countAdjacentMinesHelper(SweeperNode* node);
    void revealAllMines();
    void revealNode(SweeperNode* node);
    void handlePauseAndTakeNextAction();
    void handleWinMechanics();
};

#endif // SWEEPER_GAME_H
