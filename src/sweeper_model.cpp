#include "../inc/sweeper_common_functions.h"
#include "../inc/sweeper_model.h"

SweeperModel::SweeperModel(int height, int width, int mines)
{
    // The game isn't ready to play yet.
    this->gameState = SweeperModel::Processing;

    // NOTE: No error checking is performed here since it is up to the SweeperControlWindow to validate the parameters
    // before the game object and model have been created.
    this->height = height;
    this->width = width;
    this->mines = mines;

    // Create the nodes.
    int nodeCount = height * width;
    for(int i = 0; i < nodeCount; i++)
    {
        nodes.push_back(new SweeperNode());
    }

    // Randomly assign mines to the nodes that we just created.
    int row, col;
    for(int i = 0; i < mines; i++)
    {
        // Get random values for the row and column.
        row = SweeperCommonFunctions::getRandValInclusive(0, height - 1);
        col = SweeperCommonFunctions::getRandValInclusive(0, width - 1);

        // If the randomly chosen location is already mined then we'll need to choose another one.
        while(getNode(row, col)->mined)
        {
            // We'll keep choosing locations until we find one that's empty.
            row = SweeperCommonFunctions::getRandValInclusive(0, height - 1);
            col = SweeperCommonFunctions::getRandValInclusive(0, width - 1);
        }

        // We'll now assign a mine to the node at this location.
        SweeperNode *node = getNode(row, col);
        node->mined = true;
    }
}

SweeperModel::~SweeperModel()
{
    for(std::size_t i = 0; i < nodes.size(); i++)
    {
        delete nodes[i];
    }
}

SweeperNode* SweeperModel::getNode(int row, int col)
{
    int index = row * width + col;
    return nodes[index];
}
