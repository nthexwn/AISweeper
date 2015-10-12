#ifndef SWEEPER_GAME_H
#define SWEEPER_GAME_H

#include "sweeper_model.h"
#include "sweeper_widget.h"

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

#endif // SWEEPER_GAME_H
