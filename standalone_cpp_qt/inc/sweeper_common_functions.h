#ifndef SWEEPER_COMMON_FUNCTIONS_H
#define SWEEPER_COMMON_FUNCTIONS_H

#include <QWidget>

class SweeperCommonFunctions
{
public:
    static int getRandValInclusive(int low, int high);
    static void setWidgetPaletteColors(QWidget* widget, QColor baseColor, QColor textColor);

private:
    // __thread is a thread_local keyword that only works for POD types.  The official 'threadlocal' keyword cannot be
    // used because it is not supported by Apple (Clang will throw a compile-time error).
    static __thread bool qrandSeeded;
};

#endif // SWEEPER_COMMON_FUNCTIONS_H
