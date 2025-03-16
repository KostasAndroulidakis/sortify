#include "../include/audio_fingerprint.hpp"
#include "../include/logger.hpp"
#include <vector>
#include <unordered_map>
#include <cmath>

namespace sortify {
namespace audio {

Result<std::unordered_map<uint32_t, std::vector<FingerprintHash>>> createFingerprint(
    const std::vector<Peak>& peaks, 
    int songId
) {
    std::unordered_map<uint32_t, std::vector<FingerprintHash>> fingerprint;
    
    if (peaks.empty()) {
        return Result<std::unordered_map<uint32_t, std::vector<FingerprintHash>>>::createFailure(
            "Empty peaks vector provided");
    }
    
    if (songId < 0) {
        return Result<std::unordered_map<uint32_t, std::vector<FingerprintHash>>>::createFailure(
            "Invalid song ID: " + std::to_string(songId));
    }
    
    Logger::info("Creating fingerprint with " + std::to_string(peaks.size()) + " peaks");
    
    // Parameters for target zone
    // The target zone defines a time-frequency area where we look for peaks to pair with our anchor
    // This constellation approach makes the fingerprint robust to noise and distortion
    const float targetTimeRange = 3.0f;     // Look for targets within 3 time units
    const float minTargetTimeDelta = 0.5f;  // Minimum time between anchor and target
    const float maxFreqDelta = 30.0f;       // Maximum frequency difference
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
            // 32-bit hash structure:
            // - Bits 22-31 (10 bits): Anchor frequency (0-1023)
            // - Bits 12-21 (10 bits): Target frequency (0-1023)
            // - Bits 0-11  (12 bits): Time delta * 10 (0-4095)
            // 
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
    
    if (fingerprint.empty()) {
        return Result<std::unordered_map<uint32_t, std::vector<FingerprintHash>>>::createFailure(
            "Failed to create any fingerprint hashes");
    }
    
    Logger::info("Created fingerprint with " + std::to_string(fingerprint.size()) + " unique hashes");
    
    return Result<std::unordered_map<uint32_t, std::vector<FingerprintHash>>>::createSuccess(std::move(fingerprint));
}

} // namespace audio
} // namespace sortify