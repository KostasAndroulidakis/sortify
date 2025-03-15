#!/bin/bash
set -e # Exit on error

# Define usage function
usage() {
    echo "Usage: $0 [OPTION]"
    echo "Build and test the Sortify application"
    echo ""
    echo "Options:"
    echo "  -b, --build          Build only (default)"
    echo "  -t, --test           Run synthetic test after build"
    echo "  -f, --file FILE      Test with specified audio file"
    echo "  -h, --help           Display this help message"
    exit 1
}

# Default values
BUILD=true
TEST=false
AUDIO_FILE=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build)
            BUILD=true
            shift
            ;;
        -t|--test)
            TEST=true
            shift
            ;;
        -f|--file)
            TEST=true
            AUDIO_FILE="$2"
            shift 2
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

# Check for dependencies if testing with audio file
if [[ -n "$AUDIO_FILE" ]]; then
    # Ensure libsndfile is installed
    if ! brew list libsndfile &>/dev/null; then
        echo "libsndfile not found. Installing..."
        brew install libsndfile
    fi
fi

# Build stage
if [[ "$BUILD" == true ]]; then
    echo "=== Building Sortify ==="
    
    # Create build directory if it doesn't exist
    mkdir -p src/cpp/build
    
    # Navigate to build directory
    cd src/cpp/build
    
    # Configure with CMake
    echo "Configuring with CMake..."
    cmake ..
    
    # Build
    echo "Compiling..."
    make -j4
    
    echo "Build successful!"
    
    cd ../../..  # Return to project root
fi

# Test stage
if [[ "$TEST" == true ]]; then
    echo "=== Running Tests ==="
    
    # Navigate to build directory
    cd src/cpp/build 2>/dev/null || (echo "Build directory not found. Run with --build first." && exit 1)
    
    if [[ -n "$AUDIO_FILE" ]]; then
        if [[ -f "./audio_file_test" ]]; then
            echo "Testing with audio file: $AUDIO_FILE"
            ./audio_file_test "$AUDIO_FILE"
        else
            echo "Error: audio_file_test executable not found"
            echo "Make sure you've built the project correctly"
            exit 1
        fi
    else
        if [[ -f "./fingerprint_test" ]]; then
            echo "Running synthetic audio test..."
            ./fingerprint_test
        else
            echo "Error: fingerprint_test executable not found"
            echo "Make sure you've built the project correctly"
            exit 1
        fi
    fi
fi

# If no options were specified, show usage
if [[ "$BUILD" == false && "$TEST" == false ]]; then
    usage
fi

exit 0
