#include <iostream>
#include "sweepermodel.h"

int main()
{
    SweeperModel *sweeperModel = new SweeperModel(10, 10, 20);
    std::cout << "Hello World!" << std::endl;
    delete sweeperModel;
    std::cout << "Goodbye World!" << std::endl;
    return 0;
}
