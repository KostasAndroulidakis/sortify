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

// Placeholder implementations for the other functions
// These will be implemented in future steps

std::vector<Peak> extractPeaks(const Spectrogram& spectrogram) {
    // Placeholder - will be implemented next
    return std::vector<Peak>();
}

std::unordered_map<uint32_t, std::vector<FingerprintHash>> createFingerprint(
    const std::vector<Peak>& peaks, 
    int songId
) {
    // Placeholder - will be implemented after extractPeaks
    return std::unordered_map<uint32_t, std::vector<FingerprintHash>>();
}

} // namespace audio
} // namespace sortify