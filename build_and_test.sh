#!/bin/bash
set -e

echo "Building Sortify audio fingerprinting library..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build
echo "Compiling..."
make -j4

# Download test files if needed
cd ../src/cpp/tests
if [ ! -f "sample_audio.mp3" ] || [ ! -f "sample_audio.m4a" ]; then
    echo "Downloading test audio files..."
    bash download_audifile.sh
fi

# Run tests
echo "Running tests..."
cd ../../../build/src/cpp/tests
./audio_comparison_test

echo "All done!"
