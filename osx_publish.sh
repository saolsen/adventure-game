#!/usr/bin/env bash
# Build it first.

curl -L -o butler.zip https://broth.itch.ovh/butler/darwin-amd64/LATEST/archive/default
unzip butler.zip
chmod +x butler
./butler -V
./butler push game-osx.zip steveolsen/adventure-game:osx-latest
