cmake_minimum_required(VERSION 3.14)
project(SortifyTests VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FFTW path for macOS
set(FFTW3_INCLUDE_DIRS "/opt/homebrew/include")
set(FFTW3_LIBRARIES "/opt/homebrew/lib/libfftw3f.dylib")

# Include FetchContent for Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Add the audio fingerprint library
add_library(audio_fingerprint STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/src/spectrogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/src/peak_extraction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/src/fingerprint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/src/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/src/audio_reader.cpp
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/include
    ${FFTW3_INCLUDE_DIRS}
)

# Add the audio comparison test
add_executable(audio_comparison_test
    cpp/audio_comparison_test.cpp
)

# Link with Google Test and audio_fingerprint library
target_link_libraries(audio_fingerprint ${FFTW3_LIBRARIES})
target_link_libraries(audio_comparison_test
    gtest_main
    audio_fingerprint
)

# Add the test to CTest
enable_testing()
add_test(NAME AudioComparisonTest COMMAND audio_comparison_test)
