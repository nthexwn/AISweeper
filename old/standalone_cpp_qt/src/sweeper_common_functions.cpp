#include <QTime>
#include "../inc/sweeper_common_functions.h"

__thread bool SweeperCommonFunctions::qrandSeeded = false;

int SweeperCommonFunctions::getRandValInclusive(int low, int high)
{
    if(!qrandSeeded)
    {
        uint time = (uint)(QTime::currentTime().msec());
        uint threadLocalValue = reinterpret_cast<std::uintptr_t>(&qrandSeeded);
        uint seedValue = time * threadLocalValue;
        qsrand(seedValue);
        qrandSeeded = true;
    }
    int rand = qrand() % ((high + 1) - low) + low;
    return rand;
}

void SweeperCommonFunctions::setWidgetPaletteColors(QWidget* widget, QColor baseColor, QColor textColor)
{
    if(widget == nullptr) return;
    QPalette palette;
    palette.setColor(QPalette::Base, baseColor);
    palette.setColor(QPalette::Text, textColor);
    widget->setPalette(palette);
}
