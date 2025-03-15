cmake_minimum_required(VERSION 3.14)
project(Sortify VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set FFTW paths explicitly for Apple Silicon Mac
set(FFTW3_INCLUDE_DIRS "/opt/homebrew/include")
set(FFTW3_LIBRARIES "/opt/homebrew/lib/libfftw3f.dylib")

# libsndfile will be needed for audio file reading, but not included yet

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FFTW3_INCLUDE_DIRS}
)

# Add the audio fingerprinting library
add_library(audio_fingerprint
    src/spectrogram.cpp
    src/peak_extraction.cpp
    src/fingerprint.cpp
)

# Link FFTW library
target_link_libraries(audio_fingerprint ${FFTW3_LIBRARIES})