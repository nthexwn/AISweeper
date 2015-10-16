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

signals:
    void triggerFlagAction(QPoint);
    void triggerRevealAction(QPoint);
    void triggerRevealAdjacentAction(QPoint);

public slots:
    void disableInput();
    void enableInput();

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    bool inputEnabled;
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
    QPoint calcCursorNodeIndex();
    SweeperNode* calcNodeAtCursor();
};

#endif // SWEEPER_WIDGET_H
