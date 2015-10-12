#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

class CommonFunctions
{
public:
    static int getRandValInclusive(int low, int high);

private:
    static bool qrandSeeded;
};

#endif // COMMON_FUNCTIONS_H
