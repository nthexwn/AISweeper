#include <QApplication>
#include "controlwindow.h"

int main(int argc, char *argv[])
{   
    QApplication app(argc, argv);

    ControlWindow controlWindow;
    controlWindow.setWindowTitle("AISweeper Controller");
    controlWindow.show();

    return app.exec();
}
