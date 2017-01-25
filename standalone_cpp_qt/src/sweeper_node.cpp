#include "../inc/sweeper_node.h"

SweeperNode::SweeperNode(int col, int row)
{
    nodeState = HIDDEN;
    mined = false;
    this->col = col;
    this->row = row;
}
