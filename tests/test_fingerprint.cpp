#include "../src/include/audio_fingerprint.hpp"
#include <iostream>
#include <vector>
#include <cmath>

// Simple test function to generate a sine wave with specified frequency
std::vector<float> generateTestAudio(float frequency, float duration, int sampleRate) {
    int numSamples = static_cast<int>(duration * sampleRate);
    std::vector<float> audio(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        audio[i] = std::sin(2.0f * M_PI * frequency * t);
    }
    
    return audio;
}

// Test function for the entire fingerprinting process
bool testFingerprinting() {
    // Generate a test audio signal (440 Hz sine wave, 1 second duration, 44.1 kHz sample rate)
    std::vector<float> testAudio = generateTestAudio(440.0f, 1.0f, 44100);
    
    // Test parameters
    int sampleRate = 44100;
    int songId = 1;
    
    // Process the test audio through the fingerprinting pipeline
    try {
        // Step 1: Generate spectrogram
        sortify::audio::Spectrogram spectrogram = 
            sortify::audio::generateSpectrogram(testAudio, sampleRate);
        
        std::cout << "Spectrogram generated: " 
                  << spectrogram.size() << " frequency bins, " 
                  << (spectrogram.empty() ? 0 : spectrogram[0].size()) << " time windows\n";
        
        // Step 2: Extract peaks
        std::vector<sortify::audio::Peak> peaks = 
            sortify::audio::extractPeaks(spectrogram, sampleRate);
        
        std::cout << "Peaks extracted: " << peaks.size() << " peaks\n";
        
        // Print first few peaks
        int peaksToPrint = std::min(5, static_cast<int>(peaks.size()));