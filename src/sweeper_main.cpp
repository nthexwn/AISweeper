#include <QApplication>
#include "../inc/sweeper_control_window_old.h"

int main(int argc, char* argv[])
{   
    QApplication app(argc, argv);

    SweeperControlWindowOld sweeperControlWindowOld;
    sweeperControlWindowOld.setWindowTitle("AISweeper Controller Old");
    sweeperControlWindowOld.show();

    return app.exec();
}
