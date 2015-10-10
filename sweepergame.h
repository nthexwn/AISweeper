#ifndef SWEEPERGAME_H
#define SWEEPERGAME_H

#include "sweepermodel.h"
#include "sweeperwidget.h"

class SweeperGame : public QObject
{
    Q_OBJECT
public:
    SweeperGame();
    ~SweeperGame();
    void startGame();

private:
    SweeperModel *sweeperModel;
    SweeperWidget *sweeperWidget;
};

#endif // SWEEPERGAME_H
