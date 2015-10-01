#include <QTime>
#include "sweepermodel.h"

int SweeperModel::getRandomValue(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

void SweeperModel::assignMinesToModel(SweeperModel *sweeperModel)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    int minimumRow = 0;
    int maximumRow = sweeperModel->height;
    int row = getRandomValue(minimumRow, maximumRow);
    int minimumColumn = 0;
    int maximumColumn = sweeperModel->width;
    int column = getRandomValue(minimumColumn, maximumColumn);
    for(int i = 0; i < sweeperModel->mines; i++)
    {
        // If the randomly chosen location is already mined then we'll need to choose another one.
        while(sweeperModel->getNode(row, column).mined)
        {
            // We'll keep choosing locations until we find one that's empty.
            row = getRandomValue(minimumRow, maximumRow);
            column = getRandomValue(minimumColumn, maximumColumn);
        }

        // We'll now assign a mine to the node at this location.
        SweeperNode node = sweeperModel->getNode(row, column);
        node.mined = true;
    }
}

SweeperModel::SweeperModel(short height, short width, short mines)
{
    gameState = Loading;
    if(height < 1)
    {
        gameState = Error_Height;
        return;
    }
    else
    {
        this->height = height;
    }

    if(width < 1)
    {
        gameState = Error_Width;
        return;
    }
    else
    {
        this->width = width;
    }

    if(mines > height * width - 1)
    {
        gameState = Error_Mines;
        return;
    }
    else
    {
        this->mines = mines;
    }

    for(short i = 0; i < height * width; i++)
    {
        nodes.push_back(new SweeperNode());
    }

    assignMinesToModel(this);
}

SweeperModel::~SweeperModel()
{
    for(std::size_t i = 0; i < nodes.size(); i++)
    {
        delete nodes[i];
    }
}

SweeperNode& SweeperModel::getNode(short row, short column)
{
    short index = row * width + column;
    return *nodes[index];
}
