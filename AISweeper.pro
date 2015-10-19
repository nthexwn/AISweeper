#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T16:03:37
# Author = Alexander Johnson (nthexwn@gmail.com)
#
#-------------------------------------------------

CONFIG += \
    c++14

FORMS +=

HEADERS += \
    inc/sweeper_common_functions.h \
    inc/sweeper_game.h \
    inc/sweeper_model.h \
    inc/sweeper_node.h \
    inc/sweeper_widget.h \
    inc/sweeper_control_window_old.h

QT += \
    core gui
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += \
    rsc/sweeper_resources.qrc

SOURCES += \
    src/sweeper_common_functions.cpp \
    src/sweeper_game.cpp \
    src/sweeper_main.cpp \
    src/sweeper_model.cpp \
    src/sweeper_node.cpp \
    src/sweeper_widget.cpp \
    src/sweeper_control_window_old.cpp

TARGET += \
    AISweeper

TEMPLATE += \
    app
