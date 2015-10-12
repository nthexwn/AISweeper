#ifndef SWEEPER_NODE_H
#define SWEEPER_NODE_H

// Information regarding the state of an individual node on the game grid.
class SweeperNode
{
public:
    // Possible node states from a player's perspective.
    enum NODE_STATE
    {
        Hidden,
        Flagged,
        Detonated,
        ByNone,
        ByOne,
        ByTwo,
        ByThree,
        ByFour,
        ByFive,
        BySix,
        BySeven,
        ByEight
    };
    NODE_STATE nodeState;
    bool mined;
    bool nodeAlreadyChecked;
    SweeperNode();
};

#endif // SWEEPER_NODE_H
