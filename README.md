# Sortify (Work In Progress)

Music organization tool with audio fingerprinting to identify duplicates across formats and quality levels. **This project is under active development and not yet ready for production use.**

## Overview

Sortify helps organize music collections by identifying and comparing music files across different formats and quality levels. The application detects duplicates, compares audio quality metrics, retrieves metadata, and provides playback functionality.

## Progress

- [X] Audio Fingerprinting (C++)
  - [X] Function 1: generateSpectrogram - Convert raw audio to time-frequency representation
  - [X] Function 2: extractPeaks - Extract distinctive frequency peaks from spectrogram
  - [X] Function 3: createFingerprint - Generate fingerprint hashes from peak relationships
- [ ] Audio quality comparison across formats
- [ ] Web scraping for metadata and ISRC codes
- [ ] Duplicate detection and management
- [ ] Music player interface
- [ ] Cross-platform compatibility

## Architecture

The project uses a multi-language architecture:
- C++: Audio fingerprinting and DSP
- Rust: Coordinator layer
- Python: Metadata and web scraping
- Swift/SwiftUI: Frontend UI and audio playback

## Status

**Work in Progress**: This project is in early development with the following status:

- Initial C++ audio fingerprinting engine is functional and tested
- Architecture is defined but most components are not yet implemented
- The application is not yet usable for end users
- Development is ongoing - expect frequent changes

Contributions and feedback are welcome, but please note that APIs and architecture may change significantly as development progresses.