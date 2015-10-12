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
    static void unhookResources();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    static bool resourceHookStarted;
    static bool resourcesHooked;
    static QPixmap* tileCorrect;
    static QPixmap* tileDetonated;
    static QPixmap* tileFlagged;
    static QPixmap* tileHidden;
    static QPixmap* tileIncorrect;
    static QPixmap* tileMissedMine;
    static QPixmap* tilePushed;
    static QPixmap* tileRevealed;
    static QPixmap* tileRevealedEight;
    static QPixmap* tileRevealedFive;
    static QPixmap* tileRevealedFour;
    static QPixmap* tileRevealedOne;
    static QPixmap* tileRevealedSeven;
    static QPixmap* tileRevealedSix;
    static QPixmap* tileRevealedThree;
    static QPixmap* tileRevealedTwo;
    SweeperModel* sweeperModel;
};

#endif // SWEEPER_WIDGET_H
