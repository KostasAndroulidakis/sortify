#include "../include/audio_fingerprint.hpp"
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
// Use explicit path for M1/M2 Mac with Homebrew
#include </opt/homebrew/include/fftw3.h>

namespace sortify {
namespace audio {

// Helper function to create a Hamming window
std::vector<float> createHammingWindow(unsigned int size) {
    std::vector<float> window(size);
    for (unsigned int i = 0; i < size; ++i) {
        // Hamming window formula: 0.54 - 0.46 * cos(2π * i / (N-1))
        window[i] = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (size - 1));
    }
    return window;
}

// FFTW-based FFT implementation
void fft(std::vector<std::complex<float>>& x) {
    const size_t N = x.size();
    if (N <= 1) return;
    
    // Allocate input/output arrays for FFTW
    fftwf_complex* in = fftwf_alloc_complex(N);
    fftwf_complex* out = fftwf_alloc_complex(N);
    
    // Copy input data
    for (size_t i = 0; i < N; i++) {
        in[i][0] = x[i].real();
        in[i][1] = x[i].imag();
    }
    
    // Create plan and execute FFT
    fftwf_plan plan = fftwf_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    
    // Copy result back to input vector
    for (size_t i = 0; i < N; i++) {
        x[i] = std::complex<float>(out[i][0], out[i][1]);
    }
    
    // Clean up
    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
}

Spectrogram generateSpectrogram(
    const std::vector<AudioSample>& samples,
    unsigned int sampleRate,
    unsigned int windowSize,
    float overlap,
    float minFreq,
    float maxFreq
) {
    if (samples.empty()) {
        return Spectrogram();
    }
    
    // Calculate step size between windows based on overlap
    unsigned int stepSize = static_cast<unsigned int>(windowSize * (1.0f - overlap));
    if (stepSize == 0) stepSize = 1; // Prevent division by zero
    
    // Calculate number of windows
    unsigned int numWindows = (samples.size() - windowSize) / stepSize + 1;
    
    // Create Hamming window
    std::vector<float> hammingWindow = createHammingWindow(windowSize);
    
    // Calculate frequency range to keep
    unsigned int binSize = sampleRate / windowSize;
    unsigned int minBin = static_cast<unsigned int>(std::ceil(minFreq / binSize));
    unsigned int maxBin = static_cast<unsigned int>(std::floor(maxFreq / binSize));
    
    // Ensure valid bin range
    minBin = std::max(minBin, 0u);
    maxBin = std::min(maxBin, windowSize / 2u);
    unsigned int numBins = maxBin - minBin + 1;
    
    // Initialize spectrogram
    Spectrogram spectrogram(numBins, std::vector<float>(numWindows, 0.0f));
    
    // Process each window
    for (unsigned int windowIdx = 0; windowIdx < numWindows; ++windowIdx) {
        // Extract window from audio samples
        std::vector<std::complex<float>> windowSamples(windowSize);
        for (unsigned int i = 0; i < windowSize; ++i) {
            unsigned int sampleIdx = windowIdx * stepSize + i;
            if (sampleIdx < samples.size()) {
                // Apply Hamming window
                windowSamples[i] = std::complex<float>(samples[sampleIdx] * hammingWindow[i], 0.0f);
            } else {
                windowSamples[i] = std::complex<float>(0.0f, 0.0f); // Zero-padding
            }
        }
        
        // Apply FFT
        fft(windowSamples);
        
        // Extract magnitude for the frequency bins we care about
        for (unsigned int binIdx = 0; binIdx < numBins; ++binIdx) {
            unsigned int sourceBinIdx = minBin + binIdx;
            if (sourceBinIdx < windowSamples.size() / 2) {
                // Calculate magnitude (absolute value) of complex FFT result
                float magnitude = std::abs(windowSamples[sourceBinIdx]);
                
                // Store in spectrogram
                spectrogram[binIdx][windowIdx] = magnitude;
            }
        }
    }
    
    return spectrogram;
}

std::vector<Peak> extractPeaks(const Spectrogram& spectrogram) {
    if (spectrogram.empty() || spectrogram[0].empty()) {
        return std::vector<Peak>();
    }
    
    const unsigned int numFreqBins = spectrogram.size();
    const unsigned int numTimeWindows = spectrogram[0].size();
    
    // Define logarithmic frequency bands (Hz)
    // These bands mimic human ear sensitivity
    const std::vector<std::pair<unsigned int, unsigned int>> freqBands = {
        {0, static_cast<unsigned int>(numFreqBins * 0.1)},         // ~0-500 Hz
        {static_cast<unsigned int>(numFreqBins * 0.1), 
         static_cast<unsigned int>(numFreqBins * 0.25)},           // ~500-2000 Hz
        {static_cast<unsigned int>(numFreqBins * 0.25), 
         static_cast<unsigned int>(numFreqBins * 0.4)},            // ~2000-3000 Hz
        {static_cast<unsigned int>(numFreqBins * 0.4), 
         static_cast<unsigned int>(numFreqBins * 0.6)},            // ~3000-4000 Hz
        {static_cast<unsigned int>(numFreqBins * 0.6), 
         static_cast<unsigned int>(numFreqBins * 0.8)},            // ~4000-4500 Hz
        {static_cast<unsigned int>(numFreqBins * 0.8), numFreqBins} // ~4500-5000 Hz
    };
    
    std::vector<Peak> peaks;
    
    // Process each time window
    for (unsigned int t = 0; t < numTimeWindows; ++t) {
        std::vector<Peak> bandPeaks;
        
        // Find the maximum peak in each frequency band
        for (const auto& band : freqBands) {
            Peak maxPeak = {0.0f, static_cast<float>(t), 0.0f};
            bool foundPeak = false;
            
            // Find the maximum magnitude within this band
            for (unsigned int f = band.first; f < band.second && f < numFreqBins; ++f) {
                if (spectrogram[f][t] > maxPeak.magnitude) {
                    maxPeak.magnitude = spectrogram[f][t];
                    maxPeak.frequency = static_cast<float>(f);
                    foundPeak = true;
                }
            }
            
            if (foundPeak) {
                bandPeaks.push_back(maxPeak);
            }
        }
        
        // Calculate dynamic threshold as average of the band peaks
        float avgMagnitude = 0.0f;
        for (const auto& peak : bandPeaks) {
            avgMagnitude += peak.magnitude;
        }
        avgMagnitude /= bandPeaks.size();
        
        // Keep only peaks above the threshold
        for (const auto& peak : bandPeaks) {
            if (peak.magnitude > avgMagnitude) {
                peaks.push_back(peak);
            }
        }
    }
    
    return peaks;
}

std::unordered_map<uint32_t, std::vector<FingerprintHash>> createFingerprint(
    const std::vector<Peak>& peaks, 
    int songId
) {
    std::unordered_map<uint32_t, std::vector<FingerprintHash>> fingerprint;
    
    if (peaks.empty()) {
        return fingerprint;
    }
    
    // Parameters for target zone
    const float targetTimeRange = 3.0f;  // Look for targets within 3 time units
    const float minTargetTimeDelta = 0.5f; // Minimum time between anchor and target
    const float maxFreqDelta = 30.0f;    // Maximum frequency difference
    const unsigned int maxTargetsPerAnchor = 5; // Find up to 5 targets per anchor
    
    // For each peak (anchor), find targets in the target zone
    for (size_t i = 0; i < peaks.size(); ++i) {
        const Peak& anchor = peaks[i];
        unsigned int numTargets = 0;
        
        // Find targets in the target zone (ahead in time)
        for (size_t j = i + 1; j < peaks.size() && numTargets < maxTargetsPerAnchor; ++j) {
            const Peak& target = peaks[j];
            
            // Check if target is within time range
            float timeDelta = target.time - anchor.time;
            if (timeDelta < minTargetTimeDelta) continue;
            if (timeDelta > targetTimeRange) break; // Assuming peaks are sorted by time
            
            // Check if target is within frequency range
            float freqDelta = std::abs(target.frequency - anchor.frequency);
            if (freqDelta > maxFreqDelta) continue;
            
            // Generate 32-bit hash from anchor/target pair
            // Combine anchor frequency, target frequency, and time delta
            // The bit shifts and masks ensure each component has its own range in the hash
            uint32_t hash = (static_cast<uint32_t>(anchor.frequency) & 0x3FF) << 22 |
                           (static_cast<uint32_t>(target.frequency) & 0x3FF) << 12 |
                           (static_cast<uint32_t>(timeDelta * 10.0f) & 0xFFF);
            
            // Create fingerprint hash entry
            FingerprintHash fingerprintHash = {
                hash,
                anchor.time,
                songId
            };
            
            // Add to fingerprint
            fingerprint[hash].push_back(fingerprintHash);
            numTargets++;
        }
    }
    
    return fingerprint;
}

} // namespace audio
} // namespace sortify