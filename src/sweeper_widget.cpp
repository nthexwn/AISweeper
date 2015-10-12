#include "../inc/sweeper_widget.h"

bool SweeperWidget::resourceHookStarted = false;
bool SweeperWidget::resourcesHooked = false;
QPixmap* SweeperWidget::tileCorrect;
QPixmap* SweeperWidget::tileDetonated;
QPixmap* SweeperWidget::tileFlagged;
QPixmap* SweeperWidget::tileHidden;
QPixmap* SweeperWidget::tileIncorrect;
QPixmap* SweeperWidget::tileMissedMine;
QPixmap* SweeperWidget::tilePushed;
QPixmap* SweeperWidget::tileRevealed;
QPixmap* SweeperWidget::tileRevealedEight;
QPixmap* SweeperWidget::tileRevealedFive;
QPixmap* SweeperWidget::tileRevealedFour;
QPixmap* SweeperWidget::tileRevealedOne;
QPixmap* SweeperWidget::tileRevealedSeven;
QPixmap* SweeperWidget::tileRevealedSix;
QPixmap* SweeperWidget::tileRevealedThree;
QPixmap* SweeperWidget::tileRevealedTwo;

SweeperWidget::SweeperWidget(SweeperModel& sweeperModel, QWidget* parent) : QWidget(parent)
{
    this->sweeperModel = &sweeperModel;
    if(!resourcesHooked && !resourceHookStarted)
    {
        resourceHookStarted = true;
        hookResources();
    }
}

void SweeperWidget::hookResources()
{
    tileCorrect = new QPixmap(":/tiles/correct.png");
    tileDetonated = new QPixmap(":/tiles/detonated.png");
    tileFlagged = new QPixmap(":/tiles/flagged.png");
    tileHidden = new QPixmap(":/tiles/hidden.png");
    tileIncorrect = new QPixmap(":/tiles/incorrect.png");
    tileMissedMine = new QPixmap(":/tiles/missedMine.png");
    tilePushed = new QPixmap(":/tiles/pushed.png");
    tileRevealed = new QPixmap(":/tiles/revealed.png");
    tileRevealedEight = new QPixmap(":/tiles/revealed_eight.png");
    tileRevealedFive = new QPixmap(":/tiles/revealed_five.png");
    tileRevealedFour = new QPixmap(":/tiles/revealed_four.png");
    tileRevealedOne = new QPixmap(":/tiles/revealed_one.png");
    tileRevealedSeven = new QPixmap(":/tiles/revealed_seven.png");
    tileRevealedSix = new QPixmap(":/tiles/revealed_six.png");
    tileRevealedThree = new QPixmap(":/tiles/revealed_three.png");
    tileRevealedTwo = new QPixmap(":/tiles/revealed_two.png");
    resourcesHooked = true;
}

void SweeperWidget::mousePressEvent(QMouseEvent* event)
{

}

void SweeperWidget::mouseMoveEvent(QMouseEvent* event)
{

}

void SweeperWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    int widgetHeight = this->size().height();
    int widgetWidth = this->size().width();
    int tileHeight = widgetHeight / sweeperModel->height;
    int tileWidth  = widgetWidth / sweeperModel->width;
    QRect backgroundRect = QRect(0, 0, widgetWidth, widgetHeight);
    painter.fillRect(backgroundRect, Qt::black);
    QPixmap* tilePixmap;
    int horizStart, vertStart;
    QRect tileRect;
    SweeperNode* node;

    for(int row = 0; row < sweeperModel->height; row++)
    {
        for(int col = 0; col < sweeperModel->width; col++)
        {
            node = sweeperModel->getNode(row, col);
            switch(node->nodeState)
            {
            case SweeperNode::CORRECT:
                tilePixmap = tileCorrect;
                break;
            case SweeperNode::DETONATED:
                tilePixmap = tileDetonated;
                break;
            case SweeperNode::FLAGGED:
                tilePixmap = tileFlagged;
                break;
            case SweeperNode::HIDDEN:
                tilePixmap = tileHidden;
                // TODO: If mouse1 is here and suppressed then change this to tiles/pushed.png
                break;
            case SweeperNode::INCORRECT:
                tilePixmap = tileIncorrect;
                break;
            case SweeperNode::MISSED_MINE:
                tilePixmap = tileMissedMine;
                break;
            case SweeperNode::REVEALED:
                tilePixmap = tileRevealed;
                break;
            case SweeperNode::REVEALED_TWO:
                tilePixmap = tileRevealedTwo;
                break;
            case SweeperNode::REVEALED_THREE:
                tilePixmap = tileRevealedThree;
                break;
            case SweeperNode::REVEALED_FOUR:
                tilePixmap = tileRevealedFour;
                break;
            case SweeperNode::REVEALED_FIVE:
                tilePixmap = tileRevealedFive;
                break;
            case SweeperNode::REVEALED_SIX:
                tilePixmap = tileRevealedSix;
                break;
            case SweeperNode::REVEALED_SEVEN:
                tilePixmap = tileRevealedSeven;
                break;
            case SweeperNode::REVEALED_EIGHT:
                tilePixmap = tileRevealedEight;
                break;
            }
            horizStart = col * tileWidth;
            vertStart = row * tileHeight;
            tileRect = QRect(horizStart, vertStart, tileWidth, tileHeight);
            painter.drawPixmap(tileRect, *tilePixmap);
        }
    }
}

void SweeperWidget::unhookResources()
{
    delete tileCorrect;
    delete tileDetonated;
    delete tileFlagged;
    delete tileHidden;
    delete tileIncorrect;
    delete tileMissedMine;
    delete tilePushed;
    delete tileRevealed;
    delete tileRevealedEight;
    delete tileRevealedFive;
    delete tileRevealedFour;
    delete tileRevealedOne;
    delete tileRevealedSeven;
    delete tileRevealedSix;
    delete tileRevealedThree;
    delete tileRevealedTwo;
    resourcesHooked = false;
}
