# General
cmake_minimum_required(VERSION 3.12)
set(MY_PROJECT_NAME AISweeper)
project(${MY_PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
add_definitions("-Wall")

# Packages
find_package(X11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${X11_MODULE_PATH})
set(MY_INCLUDE_DIRS ${MY_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})
set(MY_LINK_LIBRARIES ${MY_LINK_LIBRARIES} ${X11_LIBRARIES})

# Directories
set(MY_SOURCE_DIR src/c)
set(MY_INCLUDE_DIRS ${MY_INCLUDE_DIRS} ${MY_SOURCE_DIR})

# Files
set(COMMON_SOURCE_FILES common_constants.h common_constants.c common_enums.h common_functions.h common_functions.c common_structs.h)

# Server
add_executable(server ${MY_SOURCE_DIR}/server_main.c)
target_include_directories(server PRIVATE ${MY_INCLUDE_DIRS})
set(SERVER_SOURCE_FILES ${COMMON_SOURCE_FILES} server_main.c server_game.h server_game.c server_interface.h server_interface.c server_mt19937.h server_mt19937.c)
list(TRANSFORM SERVER_SOURCE_FILES PREPEND ${MY_SOURCE_DIR}/)
target_sources(server PRIVATE ${SERVER_SOURCE_FILES})
target_link_libraries(server ${MY_LINK_LIBRARIES})

# Client target
add_executable(client ${MY_SOURCE_DIR}/client_main.c)
target_include_directories(client PRIVATE ${MY_INCLUDE_DIRS})
set(CLIENT_SOURCE_FILES ${COMMON_SOURCE_FILES} client_main.c client_game.h client_game.c client_interface.h client_interface.c)
list(TRANSFORM CLIENT_SOURCE_FILES PREPEND ${MY_SOURCE_DIR}/)
target_sources(client PRIVATE ${CLIENT_SOURCE_FILES})
target_link_libraries(client ${MY_LINK_LIBRARIES})
