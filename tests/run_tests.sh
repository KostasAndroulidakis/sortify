#!/bin/bash
# Run tests for the audio fingerprinting components

# Navigate to the C++ build directory
cd "$(dirname "$0")/../src/cpp"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake .. 
make

# Run the tests
if [ -f audio_comparison_test ]; then
  ./audio_comparison_test
else
  echo "Test executable not found. Build may have failed."
fi

# Return to the original directory
cd "$OLDPWD"
