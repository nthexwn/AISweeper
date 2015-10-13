#include "../inc/sweeper_game.h"

SweeperGame::SweeperGame(bool useGui)
{
    sweeperModel = new SweeperModel(30, 15, 10);
    this->useGui = useGui;
}

SweeperGame::~SweeperGame()
{
    delete frame;
    delete sweeperModel;
}

void SweeperGame::startGame()
{
    if(useGui)
    {
        sweeperWidget = new SweeperWidget(*sweeperModel);
        frame = new QFrame();
        frame->setFrameShape(QFrame::Panel);
        layout = new QVBoxLayout();
        layout->setMargin(0);
        frame->setLayout(layout);
        layout->addWidget(sweeperWidget);
        frame->show();
    }
}
