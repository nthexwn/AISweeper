#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T16:03:37
# Author = Alexander Johnson (nthexwn@gmail.com)
#
#-------------------------------------------------

CONFIG += \
    c++14

FORMS += \
    ui/sweeper_control_window.ui

HEADERS += \
    inc/players/player_abstract.h \
    inc/players/player_human.h \
    inc/players/player_machine_learning_ai.h \
    inc/players/player_probability_based_ai.h \
    inc/players/player_random_action_ai.h \
    inc/sweeper_batch_manager.h \
    inc/sweeper_batch_settings.h \
    inc/sweeper_batch_status.h \
    inc/sweeper_common_functions.h \
    inc/sweeper_control_window.h \
    inc/sweeper_game.h \
    inc/sweeper_model.h \
    inc/sweeper_node.h \
    inc/sweeper_widget.h

QT += \
    core gui
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += \
    rsc/sweeper_resources.qrc

SOURCES += \
    src/players/player_abstract.cpp \
    src/players/player_human.cpp \
    src/players/player_machine_learning_ai.cpp \
    src/players/player_probability_based_ai.cpp \
    src/players/player_random_action_ai.cpp \
    src/sweeper_batch_manager.cpp \
    src/sweeper_batch_settings.cpp \
    src/sweeper_batch_status.cpp \
    src/sweeper_common_functions.cpp \
    src/sweeper_control_window.cpp \
    src/sweeper_game.cpp \
    src/sweeper_main.cpp \
    src/sweeper_model.cpp \
    src/sweeper_node.cpp \
    src/sweeper_widget.cpp

TARGET += \
    AISweeper

TEMPLATE += \
    app
