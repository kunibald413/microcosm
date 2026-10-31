#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Move into build, run CMake, then build
cd build
cmake ..
cmake --build .

# Go back and run the executable
cd ..
./build/microcosm