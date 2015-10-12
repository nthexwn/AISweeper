#include "control_window.h"

ControlWindow::ControlWindow(QWidget *parent) : QMainWindow(parent)
{
    // The control window contains a central widget.
    // The central widget contains a layout.
    // The layout contains all of the other widgets.
    centralWidget = new QWidget;
    layout = new QGridLayout();
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    launchButton = new QPushButton();
    launchButton->setText("Launch");
    launchButton->connect(launchButton, SIGNAL(clicked()), this, SLOT(launchButtonClicked()));
    layout->addWidget(launchButton);
}

ControlWindow::~ControlWindow()
{
    delete launchButton;
    delete layout;
    delete sweeperGame;
}

void ControlWindow::launchButtonClicked()
{
    sweeperGame = new SweeperGame();
    sweeperGame->startGame();
}
