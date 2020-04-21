#!/usr/bin/env bash

mkdir -p build

cp -r misc/Game.app build/

clang++ -Wall -O3 -std=c++2a -I3rdparty -o build/Game.app/Contents/MacOS/game game.cpp -L3rdparty/osx -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL


