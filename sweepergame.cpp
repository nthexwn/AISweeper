#include "sweepergame.h"

SweeperGame::SweeperGame()
{
    sweeperModel = new SweeperModel(3, 3, 4);
}

SweeperGame::~SweeperGame()
{
    delete sweeperModel;
    delete sweeperWidget;
}

void SweeperGame::startGame()
{
    sweeperWidget = new SweeperWidget(*sweeperModel);
    sweeperWidget->show();
}
