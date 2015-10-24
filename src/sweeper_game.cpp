#include "../inc/sweeper_common_functions.h"
#include "../inc/sweeper_game.h"

SweeperGame::SweeperGame(SweeperBatchSettings* batchSettings, QObject* parent) : QObject(parent)
{
    this->batchSettings = batchSettings;
    return;
}

SweeperGame::~SweeperGame()
{
    delete frame; // deleting the frame also seems to delete the layout
    delete sweeperWidget;
    delete player;
    delete sweeperModel;
}

void SweeperGame::doBeginGame()
{
    firstReveal = true;
    sweeperModel = new SweeperModel(batchSettings->width, batchSettings->height, batchSettings->mines);
    switch(batchSettings->playerType)
    {
    case SweeperBatchSettings::HUMAN:
        player = new PlayerHuman(sweeperModel);
        break;
    case SweeperBatchSettings::RANDOM_ACTION_AI:
        player = new PlayerRandomActionAi(sweeperModel);
        break;
    case SweeperBatchSettings::PROBABILITY_BASED_AI:
        player = new PlayerProbabilityBasedAi(sweeperModel);
        break;
    case SweeperBatchSettings::MACHINE_LEARNING_AI:
        player = new PlayerMachineLearningAi(sweeperModel);
        break;
    }
    if(batchSettings->showGui)
    {
        sweeperWidget = new SweeperWidget(sweeperModel);
        frame = new QFrame();
        frame->setFrameShape(QFrame::Panel);
        layout = new QVBoxLayout();
        layout->setMargin(0);
        frame->setLayout(layout);
        layout->addWidget(sweeperWidget);
        connect(sweeperWidget, SIGNAL(triggerFlagAction(QPoint)), this, SLOT(doFlagAction(QPoint)));
        connect(sweeperWidget, SIGNAL(triggerRevealAction(QPoint)), this, SLOT(doRevealAction(QPoint)));
        connect(sweeperWidget, SIGNAL(triggerRevealAdjacentAction(QPoint)),
                this, SLOT(doRevealAdjacentAction(QPoint)));
        frame->show();
    }
    connect(player, SIGNAL(triggerFlagAction(QPoint)), this, SLOT(doFlagAction(QPoint)));
    connect(player, SIGNAL(triggerQuitAction()), this, SLOT(doQuitAction()));
    connect(player, SIGNAL(triggerRevealAction(QPoint)), this, SLOT(doRevealAction(QPoint)));
    connect(player, SIGNAL(triggerRevealAdjacentAction(QPoint)), this, SLOT(doRevealAdjacentAction(QPoint)));
    connect(this, SIGNAL(triggerTakeNextAction()), player, SLOT(doTakeNextAction()));
    handlePauseAndTakeNextAction();
}

void SweeperGame::doFlagAction(QPoint nodeIndex)
{
    SweeperNode* node = sweeperModel->getNode(nodeIndex.x(), nodeIndex.y());
    if(node != nullptr)
    {
        if(node->nodeState == SweeperNode::HIDDEN) node->nodeState = SweeperNode::FLAGGED;
        else if(node->nodeState == SweeperNode::FLAGGED) node->nodeState = SweeperNode::HIDDEN;
    }
    handlePauseAndTakeNextAction();
}

void SweeperGame::doQuitAction()
{
    emit triggerEndGame(this);
}

void SweeperGame::doRevealAction(QPoint nodeIndex)
{
    SweeperNode* node = sweeperModel->getNode(nodeIndex.x(), nodeIndex.y());
    if(node != nullptr)
    {
        revealNode(node);
        handleWinMechanics();
    }
    handlePauseAndTakeNextAction();
}

void SweeperGame::doRevealAdjacentAction(QPoint nodeIndex)
{
    SweeperNode* node = sweeperModel->getNode(nodeIndex.x(), nodeIndex.y());
    if(node != nullptr)
    {
        int requiredAdjacentFlags = 0;
        switch(node->nodeState)
        {
        case SweeperNode::REVEALED_ONE:
            requiredAdjacentFlags = 1;
            break;
        case SweeperNode::REVEALED_TWO:
            requiredAdjacentFlags = 2;
            break;
        case SweeperNode::REVEALED_THREE:
            requiredAdjacentFlags = 3;
            break;
        case SweeperNode::REVEALED_FOUR:
            requiredAdjacentFlags = 4;
            break;
        case SweeperNode::REVEALED_FIVE:
            requiredAdjacentFlags = 5;
            break;
        case SweeperNode::REVEALED_SIX:
            requiredAdjacentFlags = 6;
            break;
        case SweeperNode::REVEALED_SEVEN:
            requiredAdjacentFlags = 7;
            break;
        case SweeperNode::REVEALED_EIGHT:
            requiredAdjacentFlags = 8;
            break;
        default:
            break;
        }
        int adjacentFlags = 0;
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getAboveLeftNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getAboveNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getAboveRightNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getLeftNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getRightNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getBelowLeftNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getBelowNode(node));
        adjacentFlags += countAdjacentFlagsHelper(sweeperModel->getBelowRightNode(node));
        if(adjacentFlags == requiredAdjacentFlags)
        {
            bool gameOver = false;
            revealNode(sweeperModel->getAboveLeftNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getAboveNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getAboveRightNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getLeftNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getRightNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getBelowLeftNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getBelowNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) revealNode(sweeperModel->getBelowRightNode(node));
            if(sweeperModel->gameState == SweeperModel::LOST) gameOver = true;
            if(!gameOver) handleWinMechanics();
        }
    }
    handlePauseAndTakeNextAction();
}

// The returned SweeperNode::NODE_STATE is the state which we will be assigning based on the number of mines found.
SweeperNode::NODE_STATE SweeperGame::countAdjacentMines(SweeperNode* node)
{
    int mineCount = 0;
    mineCount += countAdjacentMinesHelper(sweeperModel->getAboveLeftNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getAboveNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getAboveRightNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getLeftNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getRightNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getBelowLeftNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getBelowNode(node));
    mineCount += countAdjacentMinesHelper(sweeperModel->getBelowRightNode(node));
    switch(mineCount)
    {
    case 0: return SweeperNode::REVEALED;
    case 1: return SweeperNode::REVEALED_ONE;
    case 2: return SweeperNode::REVEALED_TWO;
    case 3: return SweeperNode::REVEALED_THREE;
    case 4: return SweeperNode::REVEALED_FOUR;
    case 5: return SweeperNode::REVEALED_FIVE;
    case 6: return SweeperNode::REVEALED_SIX;
    case 7: return SweeperNode::REVEALED_SEVEN;
    case 8: return SweeperNode::REVEALED_EIGHT;
    default: return SweeperNode::INCORRECT; // Should never happen.  Indicates error.
    }
}

int SweeperGame::countAdjacentFlagsHelper(SweeperNode* node)
{
    if(node != nullptr && node->nodeState == SweeperNode::FLAGGED) return 1;
    return 0;
}

int SweeperGame::countAdjacentMinesHelper(SweeperNode* node)
{
    if(node != nullptr && node->mined) return 1;
    return 0;
}

void SweeperGame::revealAllMines()
{
    for(int i = 0; i < (int)sweeperModel->nodes.size(); i++)
    {
        SweeperNode* node = sweeperModel->nodes[i];
        switch(node->nodeState)
        {
        case SweeperNode::FLAGGED:
            if(node->mined) node->nodeState = SweeperNode::CORRECT;
            else node->nodeState = SweeperNode::INCORRECT;
            break;
        case SweeperNode::HIDDEN:
            if(node->mined) node->nodeState = SweeperNode::MISSED_MINE;
            else node->nodeState = countAdjacentMines(node);
            break;
        default:
            break;
        }
    }
}

void SweeperGame::revealNode(SweeperNode* node)
{
    if(node == nullptr || node->nodeState != SweeperNode::HIDDEN) return;
    else if(node->mined)
    {
        if(firstReveal)
        {
            // Get random values for the column and row.
            int col = SweeperCommonFunctions::getRandValInclusive(0, sweeperModel->width - 1);
            int row = SweeperCommonFunctions::getRandValInclusive(0, sweeperModel->height - 1);

            // If the randomly chosen location is already mined then we'll need to choose another one.
            while(sweeperModel->getNode(col, row)->mined)
            {
                // We'll keep choosing locations until we find one that's empty.
                col = SweeperCommonFunctions::getRandValInclusive(0, sweeperModel->width - 1);
                row = SweeperCommonFunctions::getRandValInclusive(0, sweeperModel->height - 1);
            }

            // We'll now assign a mine to the node at this location.
            SweeperNode* newNode = sweeperModel->getNode(col, row);
            newNode->mined = true;

            // We'll now remove the mine from the old location.
            node->mined = false;
        }
        else
        {
            node->nodeState = SweeperNode::DETONATED;
            sweeperModel->gameState = SweeperModel::LOST;
            revealAllMines();
            return;
        }
    }
    firstReveal = false;
    SweeperNode::NODE_STATE nodeState = countAdjacentMines(node);
    node->nodeState = nodeState;
    if(nodeState == SweeperNode::REVEALED)
    {
        revealNode(sweeperModel->getAboveLeftNode(node));
        revealNode(sweeperModel->getAboveNode(node));
        revealNode(sweeperModel->getAboveRightNode(node));
        revealNode(sweeperModel->getLeftNode(node));
        revealNode(sweeperModel->getRightNode(node));
        revealNode(sweeperModel->getBelowLeftNode(node));
        revealNode(sweeperModel->getBelowNode(node));
        revealNode(sweeperModel->getBelowRightNode(node));
    }
    return;
}

void SweeperGame::handlePauseAndTakeNextAction()
{
    QThread::msleep(batchSettings->msPausePerAction);
    emit triggerTakeNextAction();
}

void SweeperGame::handleWinMechanics()
{
    for(int i = 0; i < (int)sweeperModel->nodes.size(); i++)
    {
        SweeperNode* node = sweeperModel->nodes[i];
        if(!node->mined)
        {
            SweeperNode::NODE_STATE nodeState = node->nodeState;
            if((nodeState == SweeperNode::HIDDEN || nodeState == SweeperNode::FLAGGED)) return;
        }
    }
    sweeperModel->gameState = SweeperModel::WON;
    revealAllMines();
    return;
}
