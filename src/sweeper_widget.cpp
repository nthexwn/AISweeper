#include "../inc/sweeper_widget.h"

bool SweeperWidget::mouseOnePressed = false;
bool SweeperWidget::mouseTwoPressed = false;
bool SweeperWidget::resourceHookingStarted = false;
bool SweeperWidget::resourcesHooked = false;
QPixmap* SweeperWidget::tileCorrect;
QPixmap* SweeperWidget::tileDetonated;
QPixmap* SweeperWidget::tileFlagged;
QPixmap* SweeperWidget::tileHidden;
QPixmap* SweeperWidget::tileIncorrect;
QPixmap* SweeperWidget::tileMissedMine;
QPixmap* SweeperWidget::tilePushed;
QPixmap* SweeperWidget::tileRevealed;
QPixmap* SweeperWidget::tileRevealedOne;
QPixmap* SweeperWidget::tileRevealedTwo;
QPixmap* SweeperWidget::tileRevealedThree;
QPixmap* SweeperWidget::tileRevealedFour;
QPixmap* SweeperWidget::tileRevealedFive;
QPixmap* SweeperWidget::tileRevealedSix;
QPixmap* SweeperWidget::tileRevealedSeven;
QPixmap* SweeperWidget::tileRevealedEight;

SweeperWidget::SweeperWidget(SweeperModel& sweeperModel, QWidget* parent) : QWidget(parent)
{
    this->sweeperModel = &sweeperModel;
    if(!resourcesHooked && !resourceHookingStarted)
    {
        resourceHookingStarted = true;
        hookResources();
    }
    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);
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
    tileRevealedOne = new QPixmap(":/tiles/revealed_one.png");
    tileRevealedTwo = new QPixmap(":/tiles/revealed_two.png");
    tileRevealedThree = new QPixmap(":/tiles/revealed_three.png");
    tileRevealedFour = new QPixmap(":/tiles/revealed_four.png");
    tileRevealedFive = new QPixmap(":/tiles/revealed_five.png");
    tileRevealedSix = new QPixmap(":/tiles/revealed_six.png");
    tileRevealedSeven = new QPixmap(":/tiles/revealed_seven.png");
    tileRevealedEight = new QPixmap(":/tiles/revealed_eight.png");
    resourcesHooked = true;
}

QSize SweeperWidget::sizeHint() const
{
    int minTileWidth = tileHidden->width() / 2;
    int minTileHeight = tileHidden->height() / 2;
    int minWidgetWidth = minTileWidth * sweeperModel->width;
    int minWidgetHeight = minTileHeight * sweeperModel->height;
    return QSize(minWidgetWidth, minWidgetHeight);
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
    delete tileRevealedOne;
    delete tileRevealedTwo;
    delete tileRevealedThree;
    delete tileRevealedFour;
    delete tileRevealedFive;
    delete tileRevealedSix;
    delete tileRevealedSeven;
    delete tileRevealedEight;
    resourcesHooked = false;
}

void SweeperWidget::mousePressEvent(QMouseEvent* event)
{
    event->accept();
    Qt::MouseButton button = event->button();
    if(button & Qt::LeftButton)
    {
        mouseOnePressed = true;
        update();
    }
    else if(button & Qt::RightButton)
    {
        mouseTwoPressed = true;
        update();
    }
}

void SweeperWidget::mouseReleaseEvent(QMouseEvent* event)
{
    event->accept();
    Qt::MouseButton button = event->button();
    if(button & Qt::LeftButton)
    {
        mouseOnePressed = false;
        update();
    }
    else if(button & Qt::RightButton)
    {
        mouseTwoPressed = false;
        update();
    }
}

void SweeperWidget::paintEvent(QPaintEvent* event)
{
    event->accept();
    QPainter painter(this);
    int widgetWidth = this->size().width();
    int widgetHeight = this->size().height();
    QRect backgroundRect = QRect(0, 0, widgetWidth, widgetHeight);
    painter.fillRect(backgroundRect, Qt::black);
    int tileWidth  = widgetWidth / sweeperModel->width;
    int tileHeight = widgetHeight / sweeperModel->height;
    tileWidth = tileHeight = qMin(tileWidth, tileHeight);
    int totalTileWidth = tileWidth * sweeperModel->width;
    int totalTileHeight = tileHeight * sweeperModel->height;
    int marginWidth = (widgetWidth - totalTileWidth) / 2;
    int marginHeight = (widgetHeight - totalTileHeight) / 2;
    int horizStart, vertStart;
    QRect tileRect;
    SweeperNode* node;
    QPixmap* tilePixmap;
    for(int row = 0; row < sweeperModel->height; row++)
    {
        for(int col = 0; col < sweeperModel->width; col++)
        {
            horizStart = col * tileWidth + marginWidth;
            vertStart = row * tileHeight + marginHeight;
            tileRect = QRect(horizStart, vertStart, tileWidth, tileHeight);
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
                if(mouseOnePressed && locateNodeUnderCursor() == node)
                {
                    tilePixmap = tilePushed;
                    break;
                }
                tilePixmap = tileHidden;
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
            case SweeperNode::REVEALED_ONE:
                tilePixmap = tileRevealedOne;
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
            painter.drawPixmap(tileRect, *tilePixmap);
        }
    }
}

SweeperNode* SweeperWidget::locateNodeUnderCursor()
{
    QPoint cursorPos = QCursor::pos();
    QPoint widgetPos = mapToGlobal(QPoint(0,0));
    int widgetWidth = this->size().width();
    int widgetHeight = this->size().height();
    int tileWidth  = widgetWidth / sweeperModel->width;
    int tileHeight = widgetHeight / sweeperModel->height;
    tileWidth = tileHeight = qMin(tileWidth, tileHeight);
    int totalTileWidth = tileWidth * sweeperModel->width;
    int totalTileHeight = tileHeight * sweeperModel->height;
    int marginWidth = (widgetWidth - totalTileWidth) / 2;
    int marginHeight = (widgetHeight - totalTileHeight) / 2;
    QPoint gridPos = QPoint(marginWidth, marginHeight) + widgetPos;
    QPoint relativePos = cursorPos - gridPos;
    int row = relativePos.y() / tileHeight;
    if(row < 0 || row >= sweeperModel->height)
    {
        return NULL;
    }
    int col = relativePos.x() / tileWidth;
    if(col < 0 || col >= sweeperModel->width)
    {
        return NULL;
    }
    return sweeperModel->getNode(row, col);
}
