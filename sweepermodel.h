#ifndef SWEEPERMODEL
#define SWEEPERMODEL

#include <vector>
#include "sweepernode.h"

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
    int height;
    int width;
    int mines;
    SweeperModel(int height, int width, int mines);
    ~SweeperModel();
    SweeperNode* getNode(int row, int col);

private:
    std::vector<SweeperNode*> nodes;
};

#endif // SWEEPERMODEL
