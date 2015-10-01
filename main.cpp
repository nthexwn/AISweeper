#include <QApplication>
#include "mainwindow.h"
#include "sweepermodel.h"
#include <iostream>
#include <QTime>
#include <string>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SweeperModel *sweeperModel = new SweeperModel(16, 16, 40);
    sweeperModel->gameState = SweeperModel::Playing;
    MainWindow w;
    w.show();
    delete sweeperModel;
    return a.exec();
}
