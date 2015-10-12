#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T16:03:37
# Author = Alexander Johnson (nthexwn@gmail.com)
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AISweeper
TEMPLATE = app

HEADERS += \
    inc/sweeper_common_functions.h \
    inc/sweeper_control_window.h \
    inc/sweeper_game.h \
    inc/sweeper_model.h \
    inc/sweeper_node.h \
    inc/sweeper_widget.h

SOURCES += \
    src/sweeper_common_functions.cpp \
    src/sweeper_control_window.cpp \
    src/sweeper_game.cpp \
    src/sweeper_main.cpp \
    src/sweeper_model.cpp \
    src/sweeper_node.cpp \
    src/sweeper_widget.cpp

FORMS +=

RESOURCES += \
    rsc/sweeper_resources.qrc
