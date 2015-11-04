#ifndef SWEEPER_COMMON_FUNCTIONS_H
#define SWEEPER_COMMON_FUNCTIONS_H

#include <QWidget>

class SweeperCommonFunctions
{
public:
    static int getRandValInclusive(int low, int high);
    static void setWidgetPaletteColors(QWidget* widget, QColor baseColor, QColor textColor);

private:
    // __thread is a thread-local keyword that only works for POD types.
    static __thread bool qrandSeeded;
};

#endif // SWEEPER_COMMON_FUNCTIONS_H
