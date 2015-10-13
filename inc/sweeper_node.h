#ifndef SWEEPER_NODE_H
#define SWEEPER_NODE_H

// Information regarding the state of an individual node on the game grid.
class SweeperNode
{
public:
    // Possible node states from a player's perspective.
    enum NODE_STATE
    {
        CORRECT,
        DETONATED,
        FLAGGED,
        HIDDEN,
        INCORRECT,
        MISSED_MINE,
        REVEALED,
        REVEALED_ONE,
        REVEALED_TWO,
        REVEALED_THREE,
        REVEALED_FOUR,
        REVEALED_FIVE,
        REVEALED_SIX,
        REVEALED_SEVEN,
        REVEALED_EIGHT
    };
    NODE_STATE nodeState;
    bool mined;
    bool nodeAlreadyChecked;
    SweeperNode();
};

#endif // SWEEPER_NODE_H
