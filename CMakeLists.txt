cmake_minimum_required(VERSION 3.7)
project(AISweeper)

set(CMAKE_C_STANDARD 11)

set(CLIENT_SOURCE_FILES client_main.c client_game.h client_game.c client_interface.h client_interface.c)
set(SERVER_SOURCE_FILES server_main.c server_game.h server_game.c server_interface.h server_interface.c server_mt19937.h server_mt19937.c)
set(COMMON_SOURCE_FILES common_constants.h common_constants.c common_enums.h common_functions.h common_functions.c common_structs.h)
add_executable(client ${CLIENT_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES})