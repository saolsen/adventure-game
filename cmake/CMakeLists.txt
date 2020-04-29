cmake_minimum_required(VERSION 3.15)
project(adventure-game)

set(CMAKE_C_STANDARD 99)

include_directories(../3rdparty)
link_directories(../3rdparty/osx)

add_executable(adventure-game ../game.c)

target_link_libraries(adventure-game
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
        "raylib")