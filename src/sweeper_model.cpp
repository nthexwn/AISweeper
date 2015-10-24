#include "../inc/sweeper_common_functions.h"
#include "../inc/sweeper_model.h"

SweeperModel::SweeperModel(int width, int height, int mines)
{
    // The game isn't ready to play yet.
    this->gameState = SweeperModel::UNFINISHED;

    // NOTE: No error checking is performed here since it is up to the SweeperControlWindow to validate the parameters
    // before the game object and model have been created.
    this->width = width;
    this->height = height;
    this->mines = mines;

    // Create the nodes.
    int nodeCount = width * height;
    for(int i = 0; i < nodeCount; i++)
    {
        int col = i % width;
        int row = i / width;
        nodes.push_back(new SweeperNode(col, row));
    }

    // Randomly assign mines to the nodes that we just created.
    int col, row;
    for(int i = 0; i < mines; i++)
    {
        // Get random values for the column and row.
        col = SweeperCommonFunctions::getRandValInclusive(0, width - 1);
        row = SweeperCommonFunctions::getRandValInclusive(0, height - 1);

        // If the randomly chosen location is already mined then we'll need to choose another one.
        while(getNode(col, row)->mined)
        {
            // We'll keep choosing locations until we find one that's empty.
            col = SweeperCommonFunctions::getRandValInclusive(0, width - 1);
            row = SweeperCommonFunctions::getRandValInclusive(0, height - 1);
        }

        // We'll now assign a mine to the node at this location.
        SweeperNode* node = getNode(col, row);
        node->mined = true;
    }
}

SweeperModel::~SweeperModel()
{
    for(std::size_t i = 0; i < nodes.size(); i++)
    {
        delete nodes[i];
        nodes[i] = nullptr;
    }
}

SweeperNode* SweeperModel::getNode(int col, int row)
{
    if(col < 0 || col >= width || row < 0 || row >= height) return nullptr;
    int index = col + row * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getAboveLeftNode(SweeperNode* node)
{
    if(node->col < 1 || node->col >= width || node->row < 1 || node->row >= height) return nullptr;
    int index = node->col - 1 + (node->row - 1) * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getAboveNode(SweeperNode* node)
{
    if(node->col < 0 || node->col >= width || node->row < 1 || node->row >= height) return nullptr;
    int index = node->col + (node->row - 1) * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getAboveRightNode(SweeperNode* node)
{
    if(node->col < 0 || node->col >= width - 1 || node->row < 1 || node->row >= height) return nullptr;
    int index = node->col + 1 + (node->row - 1) * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getLeftNode(SweeperNode* node)
{
    if(node->col < 1 || node->col >= width || node->row < 0 || node->row >= height) return nullptr;
    int index = node->col - 1 + node->row * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getRightNode(SweeperNode* node)
{
    if(node->col < 0 || node->col >= width - 1 || node->row < 0 || node->row >= height) return nullptr;
    int index = node->col + 1 + node->row * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getBelowLeftNode(SweeperNode* node)
{
    if(node->col < 1 || node->col >= width || node->row < 0 || node->row >= height - 1) return nullptr;
    int index = node->col - 1 + (node->row + 1) * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getBelowNode(SweeperNode* node)
{
    if(node->col < 0 || node->col >= width || node->row < 0 || node->row >= height - 1) return nullptr;
    int index = node->col + (node->row + 1) * width;
    return nodes[index];
}

SweeperNode* SweeperModel::getBelowRightNode(SweeperNode* node)
{
    if(node->col < 0 || node->col >= width - 1 || node->row < 0 || node->row >= height - 1) return nullptr;
    int index = node->col + 1 + (node->row + 1) * width;
    return nodes[index];
}
