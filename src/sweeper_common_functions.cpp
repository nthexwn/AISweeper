#include <QTime>
#include "../inc/sweeper_common_functions.h"

bool SweeperCommonFunctions::qrandSeeded = false;

int SweeperCommonFunctions::getRandValInclusive(int low, int high)
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

void SweeperCommonFunctions::setWidgetColors(QWidget* widget, QColor baseColor, QColor textColor)
{
    if(widget == nullptr) return;
    QPalette palette;
    palette.setColor(QPalette::Base, baseColor);
    palette.setColor(QPalette::Text, textColor);
    widget->setPalette(palette);
}
