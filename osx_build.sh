#!/usr/bin/env bash

mkdir -p build

clang++ -Wall -O3 -std=c++2a -I3rdparty -o build/game game.cpp -L3rdparty/osx -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

cp -r misc/Game.app build/
mv build/game build/Game.app/Contents/MacOS/
