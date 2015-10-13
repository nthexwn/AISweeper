#ifndef SWEEPER_MODEL_H
#define SWEEPER_MODEL_H

#include <vector>
#include "sweeper_node.h"

// Abstraction of the game grid as a 1-dimensional vector along with a flag indicating game state.
class SweeperModel
{
public:
    // Possible game states from a player's perspective.
    enum GAME_STATE
    {
        Processing,
        Ready,
        Lost,
        Won,
    };
    GAME_STATE gameState;
    int width;
    int height;
    int mines;
    SweeperModel(int width, int height, int mines);
    ~SweeperModel();
    SweeperNode* getNode(int row, int col);

private:
    std::vector<SweeperNode*> nodes;
};

#endif // SWEEPER_MODEL_H
