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
        Loading,
        Error_Height,
        Error_Width,
        Error_Mines,
        Playing,
        Lost,
        Won,
        Exiting,
    };

    GAME_STATE gameState;
    short height;
    short width;
    short mines;

    int getRandomValue(int low, int high);
    void assignMinesToModel(SweeperModel *sweeperModel);
    SweeperModel(short height, short width, short mines);
    ~SweeperModel();
    SweeperNode& getNode(short row, short column);

private:
    std::vector<SweeperNode*> nodes;
};

#endif // SWEEPERMODEL
