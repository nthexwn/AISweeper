#ifndef SWEEPER_CONTROL_WINDOW_OLD_H
#define SWEEPER_CONTROL_WINDOW_OLD_H

#include <QtWidgets>
#include "sweeper_game.h"

class SweeperControlWindowOld : public QMainWindow
{
    Q_OBJECT
public:
    explicit SweeperControlWindowOld(QWidget* parent = 0);
    ~SweeperControlWindowOld();

public slots:

private:
    QWidget* centralWidget;
    QLayout* layout;
    QPushButton* launchButton;
    SweeperGame* sweeperGame;

private slots:
    void launchButtonClicked();
};

#endif // SWEEPER_CONTROL_WINDOW_OLD_H
