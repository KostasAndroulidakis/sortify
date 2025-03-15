#!/bin/bash
set -e  # Exit on error

echo "===== Building Sortify Audio Fingerprinting Tests ====="

# Go to project root
cd "$(dirname "$0")"

# Check if FFmpeg is installed (required for audio file conversion)
if ! command -v ffmpeg &> /dev/null; then
    echo "ERROR: FFmpeg is required but not installed."
    echo "Please install FFmpeg using your package manager:"
    echo "  macOS: brew install ffmpeg"
    echo "  Linux: apt-get install ffmpeg or equivalent"
    exit 1
fi

# Build and run tests
echo "Building and running tests..."
mkdir -p tests/build
cd tests/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# Run the tests if built successfully
if [ -f audio_comparison_test ]; then
    echo ""
    echo "Running audio fingerprinting tests..."
    echo "======================================"
    ./audio_comparison_test

    # Check test exit code
    if [ $? -eq 0 ]; then
        echo ""
        echo "✅ All tests passed successfully!"
    else
        echo ""
        echo "❌ Some tests failed. See above for details."
        exit 1
    fi
else
    echo "Test executable not found. Build failed."
    exit 1
fi

echo ""
echo "Tests completed."
