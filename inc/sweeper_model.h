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
        PROCESSING,
        READY,
        LOST,
        WON,
    };
    GAME_STATE gameState;
    int width;
    int height;
    int mines;
    std::vector<SweeperNode*> nodes;
    SweeperModel(int width, int height, int mines);
    ~SweeperModel();
    SweeperNode* getNode(int col, int row);
    SweeperNode* getAboveLeftNode(SweeperNode* node);
    SweeperNode* getAboveNode(SweeperNode* node);
    SweeperNode* getAboveRightNode(SweeperNode* node);
    SweeperNode* getLeftNode(SweeperNode* node);
    SweeperNode* getRightNode(SweeperNode* node);
    SweeperNode* getBelowLeftNode(SweeperNode* node);
    SweeperNode* getBelowNode(SweeperNode* node);
    SweeperNode* getBelowRightNode(SweeperNode* node);
};

#endif // SWEEPER_MODEL_H
