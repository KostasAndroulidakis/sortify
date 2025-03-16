#include "../include/audio_fingerprint.hpp"
#include "../include/logger.hpp"
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
// Use explicit path for M1/M2 Mac with Homebrew
#include </opt/homebrew/include/fftw3.h>

namespace sortify {
namespace audio {

/**
 * Helper function to create a Hamming window
 * 
 * Hamming window reduces spectral leakage during FFT by tapering 
 * signal at the edges of each segment
 * 
 * @param size Window size
 * @return Vector containing the Hamming window coefficients
 */
std::vector<float> createHammingWindow(unsigned int size) {
    std::vector<float> window(size);
    for (unsigned int i = 0; i < size; ++i) {
        // Hamming window formula: 0.54 - 0.46 * cos(2π * i / (N-1))
        window[i] = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (size - 1));
    }
    return window;
}

/**
 * Compute Fast Fourier Transform using FFTW library
 * 
 * This wrapper translates between C++ std::complex vectors and FFTW's C-style arrays.
 * FFTW is used because it's one of the fastest FFT implementations available.
 * 
 * @param x Input/output vector of complex values, modified in-place with FFT result
 * @return True if successful, false if an error occurred
 */
bool fft(std::vector<std::complex<float>>& x, std::string& errorMessage) {
    const size_t N = x.size();
    if (N <= 1) return true;  // Nothing to do for tiny arrays
    
    // Allocate input/output arrays for FFTW
    // Using FFTW's allocation functions ensures proper memory alignment for SIMD operations
    fftwf_complex* in = fftwf_alloc_complex(N);
    fftwf_complex* out = fftwf_alloc_complex(N);
    
    // Check for allocation failure
    if (!in || !out) {
        // Clean up and return error
        if (in) fftwf_free(in);
        if (out) fftwf_free(out);
        errorMessage = "Memory allocation failed for FFT";
        return false;
    }
    
    // Copy input data
    for (size_t i = 0; i < N; i++) {
        in[i][0] = x[i].real();
        in[i][1] = x[i].imag();
    }
    
    // Create plan and execute FFT
    // FFTW_ESTIMATE flag quickly creates a reasonable but non-optimal plan
    // FFTW_FORWARD indicates we're transforming from time domain to frequency domain
    fftwf_plan plan = fftwf_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    if (!plan) {
        fftwf_free(in);
        fftwf_free(out);
        errorMessage = "Failed to create FFT plan";
        return false;
    }
    
    fftwf_execute(plan);
    
    // Copy result back to input vector
    for (size_t i = 0; i < N; i++) {
        x[i] = std::complex<float>(out[i][0], out[i][1]);
    }
    
    // Clean up
    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
    
    return true;
}

/**
 * Converts raw audio samples to a time-frequency representation (spectrogram)
 * 
 * Algorithm steps:
 * 1. Divide audio into overlapping segments
 * 2. Apply window function to each segment to reduce spectral leakage
 * 3. Transform each windowed segment using FFT
 * 4. Extract magnitude information for each frequency bin
 * 5. Focus only on the desired frequency range (20Hz-5kHz)
 * 
 * @return A Result containing a 2D matrix with frequencies as rows and time windows as columns
 */
Result<Spectrogram> generateSpectrogram(
    const std::vector<AudioSample>& samples,
    unsigned int sampleRate,
    unsigned int windowSize,
    float overlap,
    float minFreq,
    float maxFreq
) {
    if (samples.empty()) {
        return Result<Spectrogram>::createFailure("Empty audio samples provided");
    }
    
    // Validate parameters
    if (sampleRate == 0) {
        return Result<Spectrogram>::createFailure("Sample rate cannot be zero");
    }
    
    if (windowSize == 0) {
        return Result<Spectrogram>::createFailure("Window size cannot be zero");
    }
    
    if (overlap < 0.0f || overlap >= 1.0f) {
        return Result<Spectrogram>::createFailure("Overlap must be between 0.0 and 1.0 (exclusive)");
    }
    
    if (minFreq < 0.0f) {
        return Result<Spectrogram>::createFailure("Minimum frequency cannot be negative");
    }
    
    if (maxFreq <= minFreq) {
        return Result<Spectrogram>::createFailure("Maximum frequency must be greater than minimum frequency");
    }
    
    // Calculate step size between windows based on overlap
    unsigned int stepSize = static_cast<unsigned int>(windowSize * (1.0f - overlap));
    if (stepSize == 0) stepSize = 1; // Prevent division by zero
    
    // Calculate number of windows
    unsigned int numWindows = (samples.size() - windowSize) / stepSize + 1;
    if (numWindows == 0) {
        return Result<Spectrogram>::createFailure("Sample size too small for given window size");
    }
    
    // Create Hamming window
    std::vector<float> hammingWindow = createHammingWindow(windowSize);
    
    // Calculate frequency range to keep
    // Each FFT bin represents a frequency range of (sample_rate / window_size) Hz
    // We convert our min/max frequency thresholds to corresponding FFT bin indices
    unsigned int binSize = sampleRate / windowSize;  // Hz per bin
    unsigned int minBin = static_cast<unsigned int>(std::ceil(minFreq / binSize));
    unsigned int maxBin = static_cast<unsigned int>(std::floor(maxFreq / binSize));
    
    // Ensure valid bin range
    minBin = std::max(minBin, 0u);
    maxBin = std::min(maxBin, windowSize / 2u);
    
    if (maxBin <= minBin) {
        return Result<Spectrogram>::createFailure("Invalid frequency range for given window size and sample rate");
    }
    
    unsigned int numBins = maxBin - minBin + 1;
    
    // Initialize spectrogram
    Spectrogram spectrogram(numBins, std::vector<float>(numWindows, 0.0f));
    
    // Log progress
    Logger::info("Generating spectrogram: " + std::to_string(numWindows) + " windows, " +
                 std::to_string(numBins) + " frequency bins");
    
    std::string fftError;
    
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
        if (!fft(windowSamples, fftError)) {
            return Result<Spectrogram>::createFailure("FFT error: " + fftError);
        }
        
        // Extract magnitude for the frequency bins we care about
        for (unsigned int binIdx = 0; binIdx < numBins; ++binIdx) {
            unsigned int sourceBinIdx = minBin + binIdx;
            if (sourceBinIdx < windowSamples.size() / 2) {
                // Calculate magnitude (absolute value) of complex FFT result
                // This converts complex value to a single real value representing amplitude
                // We ignore phase information as it's less important for fingerprinting
                float magnitude = std::abs(windowSamples[sourceBinIdx]);
                
                // Store in spectrogram
                spectrogram[binIdx][windowIdx] = magnitude;
            }
        }
    }
    
    if (spectrogram.empty() || spectrogram[0].empty()) {
        return Result<Spectrogram>::createFailure("Failed to generate spectrogram data");
    }
    
    Logger::info("Spectrogram generation complete: " + 
                std::to_string(spectrogram.size()) + "x" + 
                std::to_string(spectrogram[0].size()));
    
    return Result<Spectrogram>::createSuccess(std::move(spectrogram));
}

} // namespace audio
} // namespace sortify