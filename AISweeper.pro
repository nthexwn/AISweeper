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
    common_functions.cpp \
    control_window.cpp \
    sweeper_game.cpp \
    sweeper_model.cpp \
    sweeper_node.cpp \
    sweeper_widget.cpp

HEADERS  += \
    common_functions.h \
    control_window.h \
    sweeper_game.h \
    sweeper_model.h \
    sweeper_node.h \
    sweeper_widget.h

FORMS    +=

RESOURCES += \
    sweeper_resources.qrc
