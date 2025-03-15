#!/bin/bash
set -e  # Exit on error

# Create build directory if it doesn't exist
mkdir -p src/cpp/build

# Navigate to build directory
cd src/cpp/build

# Configure with CMake
cmake ..

# Build
make

echo ""
echo "Build successful!"
echo "You can run the test with: ./fingerprint_test"
