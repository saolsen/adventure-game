#!/usr/bin/env bash

mkdir -p game-osx

cp -r misc/Game.app game-osx/

clang++ -Wall -O3 -std=c++2a -I3rdparty -o game-osx/Game.app/Contents/MacOS/game game.cpp -L3rdparty/osx -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

zip -r game-osx.zip game-osx
