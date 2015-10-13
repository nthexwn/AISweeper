#include "../inc/sweeper_control_window.h"

SweeperControlWindow::SweeperControlWindow(QWidget* parent) : QMainWindow(parent)
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

SweeperControlWindow::~SweeperControlWindow()
{
    delete launchButton;
    delete layout;
    delete sweeperGame;
    SweeperWidget::unhookResources();
}

void SweeperControlWindow::launchButtonClicked()
{
    sweeperGame = new SweeperGame(true);
    sweeperGame->startGame();
}
