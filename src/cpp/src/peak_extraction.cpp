#include "../include/audio_fingerprint.hpp"
#include <vector>
#include <algorithm>

namespace sortify {
namespace audio {

std::vector<Peak> extractPeaks(const Spectrogram& spectrogram) {
    if (spectrogram.empty() || spectrogram[0].empty()) {
        return std::vector<Peak>();
    }
    
    const unsigned int numFreqBins = spectrogram.size();
    const unsigned int numTimeWindows = spectrogram[0].size();
    
    // Define logarithmic frequency bands (Hz)
    // These bands mimic human ear sensitivity which is more sensitive to
    // changes in lower frequencies than higher ones
    // The bands are distributed logarithmically across the spectrum
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
        // Dynamic thresholding adapts to the audio's overall volume and
        // spectral characteristics, improving fingerprint robustness across
        // different recording conditions
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

} // namespace audio
} // namespace sortify