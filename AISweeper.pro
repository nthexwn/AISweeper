#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T16:03:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AISweeper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sweepermodel.cpp \
    sweepernode.cpp

HEADERS  += mainwindow.h \
    sweepermodel.h \
    sweepernode.h

FORMS    += mainwindow.ui
