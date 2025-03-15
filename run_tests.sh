#!/bin/bash
# Root-level script to run audio fingerprinting tests

# Change to the project root directory
cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p src/cpp/build
cd src/cpp/build

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
cd -
