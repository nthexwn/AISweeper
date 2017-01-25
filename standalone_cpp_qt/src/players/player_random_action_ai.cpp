#include "../../inc/players/player_random_action_ai.h"

PlayerRandomActionAi::PlayerRandomActionAi(SweeperModel* sweeperModel, QObject* parent) :
                            PlayerAbstract(sweeperModel, parent)
{
    return;
}

void PlayerRandomActionAi::takeNextAction()
{
    if(sweeperModel->gameState == SweeperModel::LOST || sweeperModel->gameState == SweeperModel::WON)
    {
        emit triggerQuitAction();
        return;
    }
    int randCol = SweeperCommonFunctions::getRandValInclusive(0, sweeperModel->width - 1);
    int randRow = SweeperCommonFunctions::getRandValInclusive(0, sweeperModel->height - 1);
    emit triggerRevealAction(QPoint(randCol, randRow));
}
