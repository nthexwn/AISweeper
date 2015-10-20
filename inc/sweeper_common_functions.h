#ifndef SWEEPER_COMMON_FUNCTIONS_H
#define SWEEPER_COMMON_FUNCTIONS_H

#include <QWidget>

class SweeperCommonFunctions
{
public:
    static int getRandValInclusive(int low, int high);
    static void setWidgetColors(QWidget* widget, QColor baseColor, QColor textColor);

private:
    static bool qrandSeeded;
};

#endif // SWEEPER_COMMON_FUNCTIONS_H
