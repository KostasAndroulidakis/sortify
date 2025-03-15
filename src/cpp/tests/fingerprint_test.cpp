#include "../include/audio_fingerprint.hpp"
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    // Generate a simple sine wave as test data
    const int sampleRate = 44100;
    const float frequency = 440.0f; // A4 note
    const float duration = 1.0f;    // 1 second
    const int numSamples = sampleRate * duration;
    
    std::vector<float> samples(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        float time = static_cast<float>(i) / sampleRate;
        samples[i] = 0.5f * std::sin(2.0f * M_PI * frequency * time);
    }
    
    std::cout << "Generated " << samples.size() << " samples of " << frequency << "Hz sine wave" << std::endl;
    
    // Generate spectrogram
    std::cout << "Generating spectrogram..." << std::endl;
    sortify::audio::Spectrogram spectrogram = sortify::audio::generateSpectrogram(samples);
    
    // Print some basic stats about the spectrogram
    std::cout << "Spectrogram dimensions: " << spectrogram.size() << " frequency bins × " 
              << (spectrogram.empty() ? 0 : spectrogram[0].size()) << " time windows" << std::endl;
    
    // Find the peak frequency bin (should be close to our 440Hz tone)
    if (!spectrogram.empty() && !spectrogram[0].empty()) {
        size_t maxFreqBin = 0;
        float maxMagnitude = 0.0f;
        
        // Check the first time window
        for (size_t i = 0; i < spectrogram.size(); ++i) {
            if (spectrogram[i][0] > maxMagnitude) {
                maxMagnitude = spectrogram[i][0];
                maxFreqBin = i;
            }
        }
        
        // Calculate the actual frequency this bin corresponds to
        // This is a rough approximation; in a real implementation you'd need to account for the
        // frequency range limits and bin spacing
        float binFrequency = (maxFreqBin + 1) * (sampleRate / 2.0f) / spectrogram.size();
        std::cout << "Strongest frequency bin: " << maxFreqBin 
                  << " (approximately " << binFrequency << " Hz)" << std::endl;
    }
    
    std::cout << "Test completed successfully" << std::endl;
    return 0;
}