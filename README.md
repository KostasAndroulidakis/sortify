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

1. **Scan music folder** - because we need to inventory all music files and collect basic file information (format, size, existing metadata if available)
2. **Audio fingerprinting** - because we need to create unique acoustic identifiers for each song regardless of format, quality, or metadata state
3. **Retrieve metadata** - because once we have fingerprints, we can immediately identify songs and fetch accurate information from online databases
4. **Detect duplicates** - because with fingerprints and metadata, we can now reliably group identical songs across different formats and sources
5. **Compare audio quality** - because after identifying duplicates, we need to determine which version has the highest fidelity to keep
6. **Organize files** - because with duplicates identified and metadata corrected, we can rename and reorganize files according to standardized naming conventions
7. **Music playback** - because the end goal is to present the organized, deduplicated library to users with a clean interface

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
  - [ ] Audio Quality Comparison (Step 5)

- [ ] Rust Components
  - [ ] Folder Scanning (Step 1)
  - [ ] Duplicate Detection (Step 4)
  - [ ] File Organization (Step 6)
  - [ ] Cross-platform Coordinator

- [ ] Python Components
  - [ ] Metadata Retrieval (Step 3)
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