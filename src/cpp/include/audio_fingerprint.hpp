#ifndef AUDIO_FINGERPRINT_HPP
#define AUDIO_FINGERPRINT_HPP

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

// Forward declarations for other functions to be implemented later
struct Peak {
    float frequency;
    float time;
    float magnitude;
};

std::vector<Peak> extractPeaks(const Spectrogram& spectrogram);

struct FingerprintHash {
    uint32_t hash;
    float time;
    int songId;
};

std::unordered_map<uint32_t, std::vector<FingerprintHash>> createFingerprint(
    const std::vector<Peak>& peaks, 
    int songId
);

} // namespace audio
} // namespace sortify

#endif // AUDIO_FINGERPRINT_HPP