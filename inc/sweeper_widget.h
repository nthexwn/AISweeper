#ifndef SWEEPER_WIDGET_H
#define SWEEPER_WIDGET_H

#include <QtGui>
#include <QWidget>
#include "sweeper_model.h"

class SweeperWidget : public QWidget
{
    Q_OBJECT
public:
    SweeperWidget(SweeperModel &sweeperModel, QWidget* parent = 0);
    static void hookResources();
    virtual QSize sizeHint() const;
    static void unhookResources();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    SweeperNode* locateNodeUnderCursor();
    static bool mouseOnePressed;
    static bool mouseTwoPressed;
    static bool resourceHookingStarted;
    static bool resourcesHooked;
    SweeperModel* sweeperModel;
    static QPixmap* tileCorrect;
    static QPixmap* tileDetonated;
    static QPixmap* tileFlagged;
    static QPixmap* tileHidden;
    static QPixmap* tileIncorrect;
    static QPixmap* tileMissedMine;
    static QPixmap* tilePushed;
    static QPixmap* tileRevealed;
    static QPixmap* tileRevealedOne;
    static QPixmap* tileRevealedTwo;
    static QPixmap* tileRevealedThree;
    static QPixmap* tileRevealedFour;
    static QPixmap* tileRevealedFive;
    static QPixmap* tileRevealedSix;
    static QPixmap* tileRevealedSeven;
    static QPixmap* tileRevealedEight;
};

#endif // SWEEPER_WIDGET_H
