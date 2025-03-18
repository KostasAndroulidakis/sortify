# Sortify (Work In Progress)

Music organization tool with audio fingerprinting to identify duplicates across formats and quality levels. **This project is under active development and not yet ready for production use.**

## Motivation

My music folder is a mess. It has:
- Duplicate files in different formats (mp3, wav, flac)
- Files from different sources (CD rips, vinyl rips, downloads)
- Missing or wrong metadata
- Bad file names like "Track 02" or "Unknown Artist"

I'm building Sortify to fix this by automatically organizing my music collection.

## Overview

Sortify is a desktop app that:
- Finds duplicate songs using audio fingerprinting
- Keeps the highest quality version of each song
- Fixes missing and incorrect metadata
- Plays your music

## Program Logic

Sortify processes music files in this order:

1. **Scan music folder** - Collect basic file information
2. **Audio fingerprinting** - Create unique identifiers for each song
3. **Detect duplicates** - Group identical songs based on fingerprints
4. **Compare audio quality** - Identify highest quality version of each song
5. **Retrieve metadata** - Fetch accurate information for unique songs
6. **Organize files** - Rename/move files based on metadata
7. **Music playback** - Present organized library to user

## Architecture

Sortify uses a multi-language architecture to leverage each language's strengths:

- **C++**: Audio analysis and fingerprinting
  - High performance for DSP and audio processing
  - Integration with audio libraries (FFTW)

- **Rust**: Core coordinator and file system operations
  - Memory safety and concurrency control
  - Cross-platform file handling and system integration
  - Efficient communication between components

- **Python**: Metadata and web services
  - Rich ecosystem for web scraping and API integrations
  - Data processing for metadata normalization

- **Swift/SwiftUI**: User interface
  - Native macOS user experience
  - Integration with system audio services

## Progress

- [X] C++ Components
  - [X] Audio Fingerprinting (Step 2)
    - [X] Function 1: generateSpectrogram - Convert raw audio to time-frequency representation
    - [X] Function 2: extractPeaks - Extract distinctive frequency peaks from spectrogram
    - [X] Function 3: createFingerprint - Generate fingerprint hashes from peak relationships
  - [ ] Audio Quality Comparison (Step 4)

- [ ] Rust Components
  - [ ] Folder Scanning (Step 1)
  - [ ] Duplicate Detection (Step 3)
  - [ ] File Organization (Step 6)
  - [ ] Cross-platform Coordinator

- [ ] Python Components
  - [ ] Metadata Retrieval (Step 5)
    - [ ] Web scraping for accurate metadata
    - [ ] ISRC code lookup
    - [ ] Missing metadata recovery

- [ ] Swift/SwiftUI Components
  - [ ] Music Player Interface (Step 7)
  - [ ] Library Visualization
  - [ ] macOS Integration

## Notes

- **Development Status**: Early development with functional audio fingerprinting engine. Most components remain in planning stages.
- **Platform Roadmap**: Initially targeting macOS, with Windows and Linux versions planned for future releases.
- **Contributions**: Feedback and contributions welcome, though APIs and architecture may change significantly.