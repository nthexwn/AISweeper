#include <QApplication>
#include "../inc/sweeper_control_window.h"

int main(int argc, char* argv[])
{   
    QApplication app(argc, argv);

    SweeperControlWindow sweeperControlWindow;
    sweeperControlWindow.show();



    return app.exec();
}
