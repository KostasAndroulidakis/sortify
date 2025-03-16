cmake_minimum_required(VERSION 3.14)
project(Sortify VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find FFTW package (optional for FFT operations)
find_path(FFTW3_INCLUDE_DIRS fftw3.h
    PATHS /opt/homebrew/include /usr/local/include /usr/include
)
find_library(FFTW3_LIBRARIES fftw3f
    PATHS /opt/homebrew/lib /usr/local/lib /usr/lib
)
if(FFTW3_INCLUDE_DIRS AND FFTW3_LIBRARIES)
    set(FFTW3_FOUND TRUE)
    add_definitions(-DHAS_FFTW)
    message(STATUS "Found FFTW3: ${FFTW3_LIBRARIES}")
else()
    message(STATUS "FFTW3 not found, using fallback FFT implementation")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/include
    ${FFTW3_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/cpp/src/spectrogram.cpp
    src/cpp/src/peak_extraction.cpp
    src/cpp/src/fingerprint.cpp
    src/cpp/src/logger.cpp
    src/cpp/src/audio_reader.cpp
)

# Create library
add_library(audio_fingerprint STATIC ${SOURCES})
if(FFTW3_FOUND)
    target_link_libraries(audio_fingerprint ${FFTW3_LIBRARIES})
endif()

# Add subdirectory for tests
add_subdirectory(src/cpp/tests)
