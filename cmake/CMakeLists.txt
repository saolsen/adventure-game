cmake_minimum_required(VERSION 3.15)
project(adventure-game)

set(CMAKE_CXX_STANDARD 20)

include_directories(../3rdparty)
link_directories(../3rdparty/osx)

add_executable(adventure-game
        ../game.cpp
        )

target_link_libraries(adventure-game
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
        "raylib")