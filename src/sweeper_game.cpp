#include "../inc/sweeper_common_functions.h"
#include "../inc/sweeper_game.h"

SweeperGame::SweeperGame(bool showGameGui, bool unlockGameGui)
{
    firstReveal = true;
    this->showGameGui = showGameGui;
    this->unlockGameGui = unlockGameGui;
    sweeperModel = new SweeperModel(16, 16, 40);
    if(showGameGui)
    {
        sweeperWidget = new SweeperWidget(*sweeperModel);
        frame = new QFrame();
        frame->setFrameShape(QFrame::Panel);
        layout = new QVBoxLayout();
        layout->setMargin(0);
        frame->setLayout(layout);
        layout->addWidget(sweeperWidget);
        connect(this, SIGNAL(triggerInputDisabled()), sweeperWidget, SLOT(disableInput()));
        connect(this, SIGNAL(triggerInputEnabled()), sweeperWidget, SLOT(enableInput()));
        connect(sweeperWidget, SIGNAL(triggerFlagAction(QPoint)), this, SLOT(doFlagAction(QPoint)));
        connect(sweeperWidget, SIGNAL(triggerRevealAction(QPoint)), this, SLOT(doRevealAction(QPoint)));
        connect(sweeperWidget, SIGNAL(triggerRevealAdjacentAction(QPoint)), this, SLOT(doRevealAdjacentAction(QPoint)));
        frame->show();
    }
    sweeperModel->gameState = SweeperModel::READY;
}

SweeperGame::~SweeperGame()
{
    delete frame; // deleting the frame also seems to delete the layout
    delete sweeperModel;
}

void SweeperGame::doFlagAction(QPoint nodeIndex)
{
    handleActionStart();
    SweeperNode* node = sweeperModel->getNode(nodeIndex.x(), nodeIndex.y());
    if(node != NULL)
    {
        if(node->nodeState == SweeperNode::HIDDEN) node->nodeState = SweeperNode::FLAGGED;
        else if(node->nodeState == SweeperNode::FLAGGED) node->nodeState = SweeperNode::HIDDEN;
    }
    handleActionFinish();
}

void SweeperGame::doRevealAction(QPoint nodeIndex)
{
    handleActionStart();
    SweeperNode* node = sweeperModel->getNode(nodeIndex.x(), nodeIndex.y());
    if(revealNode(node)) return;
    if(node != NULL && handleWinMechanics()) return;
    handleActionFinish();
}

void SweeperGame::doRevealAdjacentAction(QPoint nodeIndex)
{
    handleActionStart();
    SweeperNode* node = sweeperModel->getNode(nodeIndex.x(), nodeIndex.y());
    if(node != NULL)
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
            if(revealNode(sweeperModel->getAboveLeftNode(node))) return;
            if(revealNode(sweeperModel->getAboveNode(node))) return;
            if(revealNode(sweeperModel->getAboveRightNode(node))) return;
            if(revealNode(sweeperModel->getLeftNode(node))) return;
            if(revealNode(sweeperModel->getRightNode(node))) return;
            if(revealNode(sweeperModel->getBelowLeftNode(node))) return;
            if(revealNode(sweeperModel->getBelowNode(node))) return;
            if(revealNode(sweeperModel->getBelowRightNode(node))) return;
            if(handleWinMechanics()) return;
        }
    }
    handleActionFinish();
}

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
    if(node != NULL && node->nodeState == SweeperNode::FLAGGED) return 1;
    return 0;
}

int SweeperGame::countAdjacentMinesHelper(SweeperNode* node)
{
    if(node != NULL && node->mined) return 1;
    return 0;
}

void SweeperGame::endGame()
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

void SweeperGame::handleActionFinish()
{
    sweeperModel->gameState = SweeperModel::READY;
    if(showGameGui && unlockGameGui) emit triggerInputEnabled();
}

void SweeperGame::handleActionStart()
{
    if(showGameGui) emit triggerInputDisabled();
    sweeperModel->gameState = SweeperModel::PROCESSING;
}

bool SweeperGame::handleWinMechanics()
{
    for(int i = 0; i < (int)sweeperModel->nodes.size(); i++)
    {
        SweeperNode* node = sweeperModel->nodes[i];
        if(!node->mined)
        {
            SweeperNode::NODE_STATE nodeState = node->nodeState;
            if((nodeState == SweeperNode::HIDDEN || nodeState == SweeperNode::FLAGGED)) return false;
        }
    }
    sweeperModel->gameState = SweeperModel::WON;
    endGame();
    return true;
}

bool SweeperGame::revealNode(SweeperNode* node)
{
    if(node == NULL || node->nodeState != SweeperNode::HIDDEN) return false;
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
            endGame();
            return true;
        }
    }
    firstReveal = false;
    SweeperNode::NODE_STATE nodeState = countAdjacentMines(node);
    node->nodeState = nodeState;
    if(nodeState == SweeperNode::REVEALED)
    {
        if(revealNode(sweeperModel->getAboveLeftNode(node))) return true;
        if(revealNode(sweeperModel->getAboveNode(node))) return true;
        if(revealNode(sweeperModel->getAboveRightNode(node))) return true;
        if(revealNode(sweeperModel->getLeftNode(node))) return true;
        if(revealNode(sweeperModel->getRightNode(node))) return true;
        if(revealNode(sweeperModel->getBelowLeftNode(node))) return true;
        if(revealNode(sweeperModel->getBelowNode(node))) return true;
        if(revealNode(sweeperModel->getBelowRightNode(node))) return true;
    }
    return false;
}
