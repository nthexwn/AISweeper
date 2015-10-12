#ifndef SWEEPER_CONTROL_WINDOW_H
#define SWEEPER_CONTROL_WINDOW_H

#include <QtWidgets>
#include "sweeper_game.h"

class SweeperControlWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SweeperControlWindow(QWidget* parent = 0);
    ~SweeperControlWindow();

public slots:

private:
    QWidget* centralWidget;
    QLayout* layout;
    QPushButton* launchButton;
    SweeperGame* sweeperGame;

private slots:
    void launchButtonClicked();
};

#endif // SWEEPER_CONTROL_WINDOW_H
