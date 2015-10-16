#ifndef SWEEPER_GAME_H
#define SWEEPER_GAME_H

#include <QFrame>
#include <QVBoxLayout>
#include "sweeper_model.h"
#include "sweeper_widget.h"

class SweeperGame : public QObject
{
    Q_OBJECT
public:
    SweeperGame(bool showGameGui, bool unlockGameGui);
    ~SweeperGame();

signals:
    void triggerInputDisabled();
    void triggerInputEnabled();

public slots:
    void doFlagAction(QPoint nodeIndex);
    void doRevealAction(QPoint nodeIndex);
    void doRevealAdjacentAction(QPoint nodeIndex);

private:
    bool firstReveal;
    bool showGameGui;
    bool unlockGameGui;
    SweeperModel* sweeperModel;
    QFrame* frame;
    QVBoxLayout* layout;
    SweeperWidget* sweeperWidget;
    SweeperNode::NODE_STATE countAdjacentMines(SweeperNode* node);
    int countAdjacentFlagsHelper(SweeperNode* node);
    int countAdjacentMinesHelper(SweeperNode* node);
    void endGame();
    void handleActionFinish();
    void handleActionStart();
    bool handleWinMechanics();
    bool revealNode(SweeperNode* node);
};

#endif // SWEEPER_GAME_H
