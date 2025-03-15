#ifndef AUDIO_FINGERPRINT_HPP
#define AUDIO_FINGERPRINT_HPP

/**
 * @file audio_fingerprint.hpp
 * @brief Audio fingerprinting implementation inspired by Shazam's algorithm
 * 
 * This module provides functions to generate audio fingerprints from raw audio data
 * using spectral analysis techniques. The implementation follows a three-stage process:
 * 1. Generate spectrogram (time-frequency representation)
 * 2. Extract distinctive frequency peaks
 * 3. Create fingerprint hashes from peak pairs
 */

#include <vector>
#include <complex>
#include <cmath>
#include <unordered_map>

namespace sortify {
namespace audio {

// Type definitions for clarity
using AudioSample = float;
using FrequencyBin = std::complex<float>;
using Spectrogram = std::vector<std::vector<float>>; // 2D matrix: rows=frequencies, cols=time

/**
 * Generates a spectrogram from raw audio data
 * 
 * @param samples Raw audio samples
 * @param sampleRate Sample rate of the audio (Hz)
 * @param windowSize Size of each window for FFT
 * @param overlap Overlap percentage between windows (0.0-1.0)
 * @param minFreq Minimum frequency to include (Hz)
 * @param maxFreq Maximum frequency to include (Hz)
 * @return 2D spectrogram where rows are frequencies and columns are time
 */
Spectrogram generateSpectrogram(
    const std::vector<AudioSample>& samples,
    unsigned int sampleRate = 44100,
    unsigned int windowSize = 2048,
    float overlap = 0.5,
    float minFreq = 20.0,
    float maxFreq = 5000.0
);

/**
 * @struct Peak
 * @brief Represents a distinctive frequency peak in a spectrogram
 * 
 * Each peak is characterized by its frequency, time position, and magnitude.
 * These peaks serve as the anchor points for fingerprint hash generation.
 */
struct Peak {
    float frequency;  ///< Frequency bin index in the spectrogram
    float time;       ///< Time position (window index) in the spectrogram
    float magnitude;  ///< Intensity/amplitude of the peak
};

/**
 * Extracts distinctive frequency peaks from a spectrogram
 * 
 * @param spectrogram 2D matrix representing the spectrogram
 * @return Vector of Peak structures representing the most distinctive points
 */
std::vector<Peak> extractPeaks(const Spectrogram& spectrogram);

/**
 * @struct FingerprintHash
 * @brief Represents a single hash in the audio fingerprint
 * 
 * Each hash is created from a pair of peaks (anchor and target)
 * and stores the anchor's time position and the associated song ID.
 */
struct FingerprintHash {
    uint32_t hash;   ///< 32-bit hash combining frequency and time information
    float time;      ///< Time position of the anchor peak
    int songId;      ///< ID of the song this hash belongs to
};

/**
 * Creates a searchable fingerprint from a collection of spectral peaks
 * 
 * The fingerprint consists of hashes created from pairs of peaks (anchor-target pairs).
 * Each hash encodes the frequencies of both peaks and their time difference.
 * These hashes are stored in a hashmap for efficient lookup during matching.
 * 
 * @param peaks Vector of spectral peaks extracted from the audio
 * @param songId Identifier for the song being fingerprinted
 * @return Hashmap where keys are 32-bit hashes and values are vectors of hash metadata
 */
std::unordered_map<uint32_t, std::vector<FingerprintHash>> createFingerprint(
    const std::vector<Peak>& peaks, 
    int songId
);

} // namespace audio
} // namespace sortify

#endif // AUDIO_FINGERPRINT_HPP