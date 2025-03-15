#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include "audio_fingerprint.hpp"

// Include our audio file reader
#include "audio_reader.hpp"

// Helper function to load audio samples from a file
std::vector<float> loadAudioFile(const std::string& filePath) {
    try {
        // Use FFmpeg-based loader for cross-format compatibility
        return sortify::audio::AudioReader::loadAudioFile(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Exception loading audio file: " << e.what() << std::endl;
        return {};
    }
}

// Helper function to measure execution time
template <typename Func>
double measureExecutionTime(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
}

// Test if two different format versions of the same song generate similar fingerprints
TEST(AudioComparisonTest, SameTrackDifferentFormats) {
    // Paths to the test files
    std::string mp3FilePath = "/Users/kanon/projects/music/Benny The Butcher - Burden Of Proof (2020) 320/05. Famous.mp3";
    std::string m4aFilePath = "/Users/kanon/projects/music/Benny the Butcher - Burden of Proof/05 Famous - Hiphopde.com.m4a";
    
    // Load audio samples with detailed error reporting
    std::cout << "Loading MP3 file..." << std::endl;
    std::vector<float> mp3Samples;
    double mp3LoadTime = measureExecutionTime([&]() {
        mp3Samples = loadAudioFile(mp3FilePath);
    });
    
    std::cout << "Loading M4A file..." << std::endl;
    std::vector<float> m4aSamples;
    double m4aLoadTime = measureExecutionTime([&]() {
        m4aSamples = loadAudioFile(m4aFilePath);
    });
    
    // Check if files loaded successfully
    if (mp3Samples.empty()) {
        std::cerr << "Failed to load MP3 file: " << mp3FilePath << std::endl;
        std::cerr << "Check if the file exists and has proper permissions." << std::endl;
        ASSERT_FALSE(mp3Samples.empty()) << "Failed to load MP3 file";
    }
    
    if (m4aSamples.empty()) {
        std::cerr << "Failed to load M4A file: " << m4aFilePath << std::endl;
        std::cerr << "Check if the file exists and has proper permissions." << std::endl;
        ASSERT_FALSE(m4aSamples.empty()) << "Failed to load M4A file";
    }
    
    // Print basic info
    std::cout << "MP3 sample count: " << mp3Samples.size() << " (loaded in " << mp3LoadTime << "s)" << std::endl;
    std::cout << "M4A sample count: " << m4aSamples.size() << " (loaded in " << m4aLoadTime << "s)" << std::endl;
    
    // Set up logging before we start
    sortify::audio::Logger::setLogLevel(sortify::audio::LogLevel::INFO);
    
    // Generate spectrograms with timing
    std::cout << "Generating spectrograms..." << std::endl;
    sortify::audio::Spectrogram mp3Spectrogram;
    double mp3SpectrogramTime = measureExecutionTime([&]() {
        auto result = sortify::audio::generateSpectrogram(mp3Samples, 44100);
        if (!result.success) {
            std::cerr << "Error generating MP3 spectrogram: " << result.errorMessage << std::endl;
            throw std::runtime_error(result.errorMessage);
        }
        mp3Spectrogram = result.value.value();
    });
    
    sortify::audio::Spectrogram m4aSpectrogram;
    double m4aSpectrogramTime = measureExecutionTime([&]() {
        auto result = sortify::audio::generateSpectrogram(m4aSamples, 44100);
        if (!result.success) {
            std::cerr << "Error generating M4A spectrogram: " << result.errorMessage << std::endl;
            throw std::runtime_error(result.errorMessage);
        }
        m4aSpectrogram = result.value.value();
    });
    
    // Check if spectrograms were generated successfully
    ASSERT_FALSE(mp3Spectrogram.empty()) << "Failed to generate MP3 spectrogram";
    ASSERT_FALSE(m4aSpectrogram.empty()) << "Failed to generate M4A spectrogram";
    
    std::cout << "MP3 spectrogram: " << mp3Spectrogram.size() << "x" << mp3Spectrogram[0].size() 
              << " (generated in " << mp3SpectrogramTime << "s)" << std::endl;
    std::cout << "M4A spectrogram: " << m4aSpectrogram.size() << "x" << m4aSpectrogram[0].size() 
              << " (generated in " << m4aSpectrogramTime << "s)" << std::endl;
    
    // Extract peaks
    std::cout << "Extracting peaks..." << std::endl;
    std::vector<sortify::audio::Peak> mp3Peaks;
    double mp3PeaksTime = measureExecutionTime([&]() {
        auto result = sortify::audio::extractPeaks(mp3Spectrogram);
        if (!result.success) {
            std::cerr << "Error extracting MP3 peaks: " << result.errorMessage << std::endl;
            throw std::runtime_error(result.errorMessage);
        }
        mp3Peaks = result.value.value();
    });
    
    std::vector<sortify::audio::Peak> m4aPeaks;
    double m4aPeaksTime = measureExecutionTime([&]() {
        auto result = sortify::audio::extractPeaks(m4aSpectrogram);
        if (!result.success) {
            std::cerr << "Error extracting M4A peaks: " << result.errorMessage << std::endl;
            throw std::runtime_error(result.errorMessage);
        }
        m4aPeaks = result.value.value();
    });
    
    // Check if peaks were extracted successfully
    ASSERT_FALSE(mp3Peaks.empty()) << "Failed to extract MP3 peaks";
    ASSERT_FALSE(m4aPeaks.empty()) << "Failed to extract M4A peaks";
    
    std::cout << "MP3 peaks: " << mp3Peaks.size() << " (extracted in " << mp3PeaksTime << "s)" << std::endl;
    std::cout << "M4A peaks: " << m4aPeaks.size() << " (extracted in " << m4aPeaksTime << "s)" << std::endl;
    
    // Create fingerprints
    std::cout << "Creating fingerprints..." << std::endl;
    std::unordered_map<uint32_t, std::vector<sortify::audio::FingerprintHash>> mp3Fingerprint;
    double mp3FingerprintTime = measureExecutionTime([&]() {
        auto result = sortify::audio::createFingerprint(mp3Peaks, 1);
        if (!result.success) {
            std::cerr << "Error creating MP3 fingerprint: " << result.errorMessage << std::endl;
            throw std::runtime_error(result.errorMessage);
        }
        mp3Fingerprint = result.value.value();
    });
    
    std::unordered_map<uint32_t, std::vector<sortify::audio::FingerprintHash>> m4aFingerprint;
    double m4aFingerprintTime = measureExecutionTime([&]() {
        auto result = sortify::audio::createFingerprint(m4aPeaks, 2);
        if (!result.success) {
            std::cerr << "Error creating M4A fingerprint: " << result.errorMessage << std::endl;
            throw std::runtime_error(result.errorMessage);
        }
        m4aFingerprint = result.value.value();
    });
    
    // Check if fingerprints were created successfully
    ASSERT_FALSE(mp3Fingerprint.empty()) << "Failed to create MP3 fingerprint";
    ASSERT_FALSE(m4aFingerprint.empty()) << "Failed to create M4A fingerprint";
    
    std::cout << "MP3 fingerprint hash count: " << mp3Fingerprint.size() 
              << " (created in " << mp3FingerprintTime << "s)" << std::endl;
    std::cout << "M4A fingerprint hash count: " << m4aFingerprint.size() 
              << " (created in " << m4aFingerprintTime << "s)" << std::endl;
    
    // Count matching hashes
    std::cout << "Analyzing fingerprint match..." << std::endl;
    int matchCount = 0;
    double matchAnalysisTime = measureExecutionTime([&]() {
        for (const auto& mp3Hash : mp3Fingerprint) {
            if (m4aFingerprint.find(mp3Hash.first) != m4aFingerprint.end()) {
                matchCount++;
            }
        }
    });
    
    float matchPercentage = 100.0f * matchCount / std::min(mp3Fingerprint.size(), m4aFingerprint.size());
    std::cout << "Matching hashes: " << matchCount << " (" << matchPercentage << "%) "
              << "(analyzed in " << matchAnalysisTime << "s)" << std::endl;
    
    // Calculate overall match scores using Shazam-like algorithm
    std::cout << "Calculating match score..." << std::endl;
    
    // Group matches by time offsets
    std::unordered_map<int, int> timeOffsetHistogram;
    
    for (const auto& [hash, mp3Hashes] : mp3Fingerprint) {
        auto it = m4aFingerprint.find(hash);
        if (it != m4aFingerprint.end()) {
            // For each matching hash, record the time offset between the MP3 and M4A versions
            for (const auto& mp3Hash : mp3Hashes) {
                for (const auto& m4aHash : it->second) {
                    // Calculate time offset in milliseconds
                    int timeOffset = static_cast<int>((mp3Hash.time - m4aHash.time) * 1000);
                    timeOffsetHistogram[timeOffset]++;
                }
            }
        }
    }
    
    // Find the most common time offset (highest count)
    int maxCount = 0;
    for (const auto& [offset, count] : timeOffsetHistogram) {
        if (count > maxCount) {
            maxCount = count;
        }
    }
    
    // Calculate a confidence score based on the highest time offset frequency
    float confidenceScore = 0.0f;
    if (!timeOffsetHistogram.empty()) {
        confidenceScore = 100.0f * maxCount / timeOffsetHistogram.size();
    }
    
    std::cout << "Match confidence score: " << confidenceScore << "%" << std::endl;
    
    // We expect at least 5% of hashes to match between the same song in different formats
    // This threshold may need adjustment based on your algorithm's performance
    EXPECT_GT(matchPercentage, 5.0f) << "Match percentage too low for the same song";
    
    // For a true match, we expect the confidence score to be reasonably high
    EXPECT_GT(confidenceScore, 20.0f) << "Confidence score too low for the same song";
}

// Test if fingerprints from the same file match themselves (sanity check)
TEST(AudioComparisonTest, SameFileSanityCheck) {
    // Path to test file
    std::string filePath = "/Users/kanon/projects/music/Benny The Butcher - Burden Of Proof (2020) 320/05. Famous.mp3";
    
    // Load audio samples
    std::vector<float> samples = loadAudioFile(filePath);
    ASSERT_FALSE(samples.empty()) << "Failed to load audio file";
    
    std::cout << "Original samples count: " << samples.size() << std::endl;
    
    // Process the first half of the file
    std::vector<float> firstHalf(samples.begin(), samples.begin() + samples.size()/2);
    std::cout << "First half samples count: " << firstHalf.size() << std::endl;
    
    // Process the full file
    std::vector<float> fullFile = samples;
    
    // Generate spectrograms
    std::cout << "Generating spectrograms..." << std::endl;
    auto firstHalfResult = sortify::audio::generateSpectrogram(firstHalf, 44100);
    ASSERT_TRUE(firstHalfResult.success) << "Failed to generate first half spectrogram: " << firstHalfResult.errorMessage;
    sortify::audio::Spectrogram firstHalfSpectrogram = firstHalfResult.value.value();
    
    auto fullFileResult = sortify::audio::generateSpectrogram(fullFile, 44100);
    ASSERT_TRUE(fullFileResult.success) << "Failed to generate full file spectrogram: " << fullFileResult.errorMessage;
    sortify::audio::Spectrogram fullFileSpectrogram = fullFileResult.value.value();
    
    // Extract peaks
    std::cout << "Extracting peaks..." << std::endl;
    auto firstHalfPeaksResult = sortify::audio::extractPeaks(firstHalfSpectrogram);
    ASSERT_TRUE(firstHalfPeaksResult.success) << "Failed to extract first half peaks: " << firstHalfPeaksResult.errorMessage;
    std::vector<sortify::audio::Peak> firstHalfPeaks = firstHalfPeaksResult.value.value();
    
    auto fullFilePeaksResult = sortify::audio::extractPeaks(fullFileSpectrogram);
    ASSERT_TRUE(fullFilePeaksResult.success) << "Failed to extract full file peaks: " << fullFilePeaksResult.errorMessage;
    std::vector<sortify::audio::Peak> fullFilePeaks = fullFilePeaksResult.value.value();
    
    std::cout << "First half peaks: " << firstHalfPeaks.size() << std::endl;
    std::cout << "Full file peaks: " << fullFilePeaks.size() << std::endl;
    
    // Create fingerprints
    std::cout << "Creating fingerprints..." << std::endl;
    auto firstHalfFingerprintResult = sortify::audio::createFingerprint(firstHalfPeaks, 1);
    ASSERT_TRUE(firstHalfFingerprintResult.success) << "Failed to create first half fingerprint: " << firstHalfFingerprintResult.errorMessage;
    auto firstHalfFingerprint = firstHalfFingerprintResult.value.value();
    
    auto fullFileFingerprintResult = sortify::audio::createFingerprint(fullFilePeaks, 2);
    ASSERT_TRUE(fullFileFingerprintResult.success) << "Failed to create full file fingerprint: " << fullFileFingerprintResult.errorMessage;
    auto fullFileFingerprint = fullFileFingerprintResult.value.value();
    
    std::cout << "First half fingerprint hash count: " << firstHalfFingerprint.size() << std::endl;
    std::cout << "Full file fingerprint hash count: " << fullFileFingerprint.size() << std::endl;
    
    // For a valid fingerprinting algorithm, fingerprints from the first half
    // should be found in the full file fingerprint
    int matchCount = 0;
    for (const auto& hashEntry : firstHalfFingerprint) {
        if (fullFileFingerprint.find(hashEntry.first) != fullFileFingerprint.end()) {
            matchCount++;
        }
    }
    
    float matchPercentage = 100.0f * matchCount / firstHalfFingerprint.size();
    std::cout << "First half to full file match percentage: " << matchPercentage << "%" << std::endl;
    
    // We expect a high percentage of matches since the full file contains the first half
    EXPECT_GT(matchPercentage, 50.0f) << "Match percentage too low for the same file";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
