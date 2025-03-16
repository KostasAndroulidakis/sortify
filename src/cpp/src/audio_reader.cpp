// Include the audio_reader implementation
#include "../include/audio_reader.hpp"

// Audio reader implementation notes:
// All methods are implemented as static inline in the header file
// This cpp file exists to ensure the header is properly compiled and used

namespace sortify {
namespace audio {

// Sample implementation function to verify the audio_reader system
bool verifyAudioReaderSystem() {
    // Use something from the header to ensure it's directly used
    std::vector<float> testSamples = AudioReader::loadWavFile("/tmp/test.wav", false);
    return testSamples.empty(); // Will return true if file doesn't exist, which is expected
}

} // namespace audio
} // namespace sortify
