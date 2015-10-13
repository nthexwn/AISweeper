#ifndef SWEEPER_GAME_H
#define SWEEPER_GAME_H

#include <QFrame>
#include <QVBoxLayout>
#include "sweeper_model.h"
#include "sweeper_widget.h"

class SweeperGame : public QObject
{
    Q_OBJECT
public:
    SweeperGame(bool useGui);
    ~SweeperGame();
    void startGame();

private:
    QFrame* frame;
    QVBoxLayout* layout;
    SweeperModel* sweeperModel;
    SweeperWidget* sweeperWidget;
    bool useGui;
};

#endif // SWEEPER_GAME_H
