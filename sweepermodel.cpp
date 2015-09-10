#include "sweepermodel.h"

SweeperModel::SweeperModel(short height, short width, short mines)
{
    gameState = GAME_STATE::Loading;
    if(height < 1)
    {
        gameState = GAME_STATE::Error_Height;
        return;
    }
    else
    {
        this->height = height;
    }

    if(width < 1)
    {
        gameState = GAME_STATE::Error_Width;
        return;
    }
    else
    {
        this->width = width;
    }

    if(mines > height * width - 1)
    {
        gameState = GAME_STATE::Error_Mines;
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
