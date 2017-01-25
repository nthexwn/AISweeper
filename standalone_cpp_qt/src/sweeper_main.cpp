#include <QApplication>
#include "../inc/sweeper_control_window.h"

int main(int argc, char* argv[])
{   
    // This object runs the main event loop for Qt.
    QApplication app(argc, argv);

    // This is the control window for our program.  It will instantiate all of our other objects internally.
    SweeperControlWindow sweeperControlWindow;

    // This tells the control window to display as soon as the main event loop starts
    sweeperControlWindow.show();

    // Start the main event loop and allow the control window to send/receive signals to/from other objects.
    return app.exec();
}
