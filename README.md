# Sortify

Music organization tool with audio fingerprinting to identify duplicates across formats and quality levels.

## Overview

Sortify helps organize music collections by:
- Using audio fingerprinting to identify songs and detect duplicates
- Comparing audio quality metrics across different formats
- Retrieving metadata via web scraping
- Providing a music player interface

## Architecture

The project uses a multi-language architecture:
- C++: Audio fingerprinting and DSP
- Rust: Coordinator layer
- Python: Metadata and web scraping
- Swift/SwiftUI: Frontend UI and audio playback

## Status

This project is in early development.

## Building

### Dependencies

- FFTW3: High-performance FFT library
  - macOS: `brew install fftw`
  - Ubuntu/Debian: `sudo apt-get install libfftw3-dev`
  - Windows: Download from [FFTW official website](http://www.fftw.org/)

### Compiling C++ components

```bash
cd src/cpp
mkdir build && cd build
cmake ..
make
```

## License

MIT