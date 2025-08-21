#!/bin/bash

echo "Building ImgCompress project..."

# Clean previous builds
echo "Cleaning previous builds..."
make clean

# Build the entire project
echo "Building with qmake6..."
qmake6 Interview_Project.pro
make

echo "Build completed!"
echo "Executable should be in: $(pwd)/build/ImgCompressUI" 