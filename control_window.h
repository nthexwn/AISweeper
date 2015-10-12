#ifndef CONTROL_WINDOW_H
#define CONTROL_WINDOW_H

#include <QtWidgets>
#include "sweeper_game.h"

class ControlWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ControlWindow(QWidget *parent = 0);
    ~ControlWindow();

public slots:

private:
    QWidget *centralWidget;
    QLayout *layout;
    QPushButton *launchButton;
    SweeperGame *sweeperGame;

private slots:
    void launchButtonClicked();
};

#endif // CONTROL_WINDOW_H
