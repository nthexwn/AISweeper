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
        UNFINISHED,
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
    SweeperNode* getNode(int col, int row) const;
    SweeperNode* getAboveLeftNode(SweeperNode* node) const;
    SweeperNode* getAboveNode(SweeperNode* node) const;
    SweeperNode* getAboveRightNode(SweeperNode* node) const;
    SweeperNode* getLeftNode(SweeperNode* node) const;
    SweeperNode* getRightNode(SweeperNode* node) const;
    SweeperNode* getBelowLeftNode(SweeperNode* node) const;
    SweeperNode* getBelowNode(SweeperNode* node) const;
    SweeperNode* getBelowRightNode(SweeperNode* node) const;
};

#endif // SWEEPER_MODEL_H
