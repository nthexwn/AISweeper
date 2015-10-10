#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QtWidgets>
#include "sweepergame.h"

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

#endif // CONTROLWINDOW_H
