#include <QTime>
#include "common_functions.h"

bool CommonFunctions::qrandSeeded = false;

int CommonFunctions::getRandValInclusive(int low, int high)
{
    if(!qrandSeeded)
    {
        QTime time = QTime::currentTime();
        qsrand((uint)time.msec());
        qrandSeeded = true;
    }
    int rand = qrand() % ((high + 1) - low) + low;
    return rand;
}
