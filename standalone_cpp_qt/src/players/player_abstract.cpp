#include "../../inc/players/player_abstract.h"

PlayerAbstract::PlayerAbstract(SweeperModel* sweeperModel, QObject* parent) : QObject(parent)
{
    this->sweeperModel = sweeperModel;
}

void PlayerAbstract::doTakeNextAction()
{
    takeNextAction();
}
