#!/bin/bash

echo "Setting up Qt environment and running ImgCompressUI..."

# Set Qt environment variables
export QT_QPA_PLATFORM_PLUGIN_PATH=$(qmake6 -query QT_INSTALL_PLUGINS)
export QT_PLUGIN_PATH=$(qmake6 -query QT_INSTALL_PLUGINS)
export LD_LIBRARY_PATH=$(qmake6 -query QT_INSTALL_LIBS):$LD_LIBRARY_PATH

# Check if executable exists
if [ ! -f "build/ImgCompressUI" ]; then
    echo "Error: ImgCompressUI executable not found!"
    echo "Please run ./build.sh first to build the project."
    exit 1
fi

echo "Starting ImgCompressUI..."
cd build
./ImgCompressUI 