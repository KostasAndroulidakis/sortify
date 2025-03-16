#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Run CMake to configure the project
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
make -j4

# Run tests if any
if [ -f "tests/runTests" ]; then
  ./tests/runTests
fi
