#ifndef SWEEPER_COMMON_FUNCTIONS_H
#define SWEEPER_COMMON_FUNCTIONS_H

class SweeperCommonFunctions
{
public:
    static int getRandValInclusive(int low, int high);

private:
    static bool qrandSeeded;
};

#endif // SWEEPER_COMMON_FUNCTIONS_H
