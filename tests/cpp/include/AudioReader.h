#ifndef AUDIO_READER_H
#define AUDIO_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <cmath>

/**
 * A simplified audio file reader for testing purposes
 * Currently supports only WAV files, just enough for our tests
 */
class AudioReader {
public:
    /**
     * Load audio samples from a WAV file
     * 
     * @param filePath Path to the audio file
     * @param normalize Whether to normalize the samples to range [-1.0, 1.0]
     * @return Vector of float samples, or empty vector if error
     */
    static std::vector<float> loadWavFile(const std::string& filePath, bool normalize = true) {
        std::ifstream file(filePath, std::ios::binary);
        
        // Check if file exists and can be opened
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not open file: " << filePath << std::endl;
            return {};
        }

        // Read file into a buffer
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> fileData(fileSize);
        file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
        file.close();
        
        if (fileSize < 44) {  // Minimum WAV header size
            std::cerr << "ERROR: File too small to be a valid WAV: " << filePath << std::endl;
            return {};
        }
        
        // Check WAV header
        if (fileData[0] != 'R' || fileData[1] != 'I' || fileData[2] != 'F' || fileData[3] != 'F') {
            std::cerr << "ERROR: Not a valid WAV file (RIFF header missing): " << filePath << std::endl;
            return {};
        }
        
        if (fileData[8] != 'W' || fileData[9] != 'A' || fileData[10] != 'V' || fileData[11] != 'E') {
            std::cerr << "ERROR: Not a valid WAV file (WAVE format missing): " << filePath << std::endl;
            return {};
        }
        
        // Find the data chunk
        int dataChunkPos = findChunk(fileData, "data");
        if (dataChunkPos == -1) {
            std::cerr << "ERROR: Could not find data chunk in WAV file: " << filePath << std::endl;
            return {};
        }
        
        // Find the format chunk
        int fmtChunkPos = findChunk(fileData, "fmt ");
        if (fmtChunkPos == -1) {
            std::cerr << "ERROR: Could not find fmt chunk in WAV file: " << filePath << std::endl;
            return {};
        }
        
        // Read format info
        uint16_t audioFormat = readUint16(fileData, fmtChunkPos + 8);
        uint16_t numChannels = readUint16(fileData, fmtChunkPos + 10);
        uint32_t sampleRate = readUint32(fileData, fmtChunkPos + 12);
        uint16_t bitsPerSample = readUint16(fileData, fmtChunkPos + 22);
        
        // Show debug info
        std::cout << "File: " << filePath << std::endl;
        std::cout << "Format: " << audioFormat << " (1=PCM, 3=IEEE Float)" << std::endl;
        std::cout << "Channels: " << numChannels << std::endl;
        std::cout << "Sample Rate: " << sampleRate << " Hz" << std::endl;
        std::cout << "Bit Depth: " << bitsPerSample << " bits" << std::endl;
        
        // Read data size
        uint32_t dataSize = readUint32(fileData, dataChunkPos + 4);
        int dataOffset = dataChunkPos + 8;
        
        if (dataOffset + dataSize > fileSize) {
            std::cerr << "ERROR: WAV file data chunk exceeds file size: " << filePath << std::endl;
            return {};
        }
        
        // Convert to samples
        std::vector<float> samples;
        samples.reserve(dataSize / (bitsPerSample / 8) / numChannels);
        
        // Process based on bit depth
        if (bitsPerSample == 16) {
            // Process 16-bit samples
            for (uint32_t i = 0; i < dataSize; i += 2 * numChannels) {
                if (dataOffset + i + 1 >= fileSize) break;
                
                // For each channel, get the sample and average them
                float sampleValue = 0.0f;
                for (uint16_t ch = 0; ch < numChannels; ++ch) {
                    if (dataOffset + i + (ch * 2) + 1 >= fileSize) break;
                    
                    int16_t sampleInt = static_cast<int16_t>(
                        (fileData[dataOffset + i + (ch * 2)] & 0xFF) |
                        ((fileData[dataOffset + i + (ch * 2) + 1] & 0xFF) << 8)
                    );
                    
                    // Convert to float [-1.0, 1.0]
                    float value = static_cast<float>(sampleInt) / 32768.0f;
                    sampleValue += value;
                }
                
                // Average
                sampleValue /= numChannels;
                samples.push_back(sampleValue);
            }
        } 
        else if (bitsPerSample == 24) {
            // Process 24-bit samples
            for (uint32_t i = 0; i < dataSize; i += 3 * numChannels) {
                if (dataOffset + i + 2 >= fileSize) break;
                
                // For each channel, get the sample and average them
                float sampleValue = 0.0f;
                for (uint16_t ch = 0; ch < numChannels; ++ch) {
                    if (dataOffset + i + (ch * 3) + 2 >= fileSize) break;
                    
                    int32_t sampleInt = 
                        (fileData[dataOffset + i + (ch * 3)] & 0xFF) |
                        ((fileData[dataOffset + i + (ch * 3) + 1] & 0xFF) << 8) |
                        ((fileData[dataOffset + i + (ch * 3) + 2] & 0xFF) << 16);
                    
                    // Sign extension for 24-bit
                    if (sampleInt & 0x800000) {
                        sampleInt |= 0xFF000000;
                    }
                    
                    // Convert to float [-1.0, 1.0]
                    float value = static_cast<float>(sampleInt) / 8388608.0f;
                    sampleValue += value;
                }
                
                // Average
                sampleValue /= numChannels;
                samples.push_back(sampleValue);
            }
        }
        else if (bitsPerSample == 32) {
            // Process 32-bit samples
            if (audioFormat == 3) {  // IEEE Float
                for (uint32_t i = 0; i < dataSize; i += 4 * numChannels) {
                    if (dataOffset + i + 3 >= fileSize) break;
                    
                    // For each channel, get the sample and average them
                    float sampleValue = 0.0f;
                    for (uint16_t ch = 0; ch < numChannels; ++ch) {
                        if (dataOffset + i + (ch * 4) + 3 >= fileSize) break;
                        
                        uint32_t sampleBits = 
                            (fileData[dataOffset + i + (ch * 4)] & 0xFF) |
                            ((fileData[dataOffset + i + (ch * 4) + 1] & 0xFF) << 8) |
                            ((fileData[dataOffset + i + (ch * 4) + 2] & 0xFF) << 16) |
                            ((fileData[dataOffset + i + (ch * 4) + 3] & 0xFF) << 24);
                        
                        // Convert to float (assuming IEEE 754)
                        float value = *reinterpret_cast<float*>(&sampleBits);
                        sampleValue += value;
                    }
                    
                    // Average
                    sampleValue /= numChannels;
                    samples.push_back(sampleValue);
                }
            } else {  // PCM 32-bit
                for (uint32_t i = 0; i < dataSize; i += 4 * numChannels) {
                    if (dataOffset + i + 3 >= fileSize) break;
                    
                    // For each channel, get the sample and average them
                    float sampleValue = 0.0f;
                    for (uint16_t ch = 0; ch < numChannels; ++ch) {
                        if (dataOffset + i + (ch * 4) + 3 >= fileSize) break;
                        
                        int32_t sampleInt = 
                            (fileData[dataOffset + i + (ch * 4)] & 0xFF) |
                            ((fileData[dataOffset + i + (ch * 4) + 1] & 0xFF) << 8) |
                            ((fileData[dataOffset + i + (ch * 4) + 2] & 0xFF) << 16) |
                            ((fileData[dataOffset + i + (ch * 4) + 3] & 0xFF) << 24);
                        
                        // Convert to float [-1.0, 1.0]
                        float value = static_cast<float>(sampleInt) / 2147483648.0f;
                        sampleValue += value;
                    }
                    
                    // Average
                    sampleValue /= numChannels;
                    samples.push_back(sampleValue);
                }
            }
        }
        else if (bitsPerSample == 8) {
            // Process 8-bit samples (usually unsigned)
            for (uint32_t i = 0; i < dataSize; i += numChannels) {
                if (dataOffset + i >= fileSize) break;
                
                // For each channel, get the sample and average them
                float sampleValue = 0.0f;
                for (uint16_t ch = 0; ch < numChannels; ++ch) {
                    if (dataOffset + i + ch >= fileSize) break;
                    
                    uint8_t sampleUInt = fileData[dataOffset + i + ch];
                    
                    // Convert to float [-1.0, 1.0] (8-bit WAV is unsigned, centered at 128)
                    float value = (static_cast<float>(sampleUInt) - 128.0f) / 128.0f;
                    sampleValue += value;
                }
                
                // Average
                sampleValue /= numChannels;
                samples.push_back(sampleValue);
            }
        }
        else {
            std::cerr << "ERROR: Unsupported bit depth: " << bitsPerSample << std::endl;
            return {};
        }
        
        std::cout << "Loaded " << samples.size() << " samples from " << filePath << std::endl;
        
        // If requested, normalize the samples
        if (normalize && !samples.empty()) {
            float maxAbs = 0.0f;
            for (const auto& sample : samples) {
                maxAbs = std::max(maxAbs, std::fabs(sample));
            }
            
            if (maxAbs > 0.0f) {
                float normFactor = 1.0f / maxAbs;
                for (auto& sample : samples) {
                    sample *= normFactor;
                }
                std::cout << "Normalized with factor: " << normFactor << std::endl;
            }
        }
        
        return samples;
    }
    
    /**
     * Load audio samples from an MP3 or M4A file using FFmpeg (requires system FFmpeg)
     * This is a more robust method that can handle various formats
     * 
     * @param filePath Path to the audio file
     * @return Vector of float samples, or empty vector if error
     */
    static std::vector<float> loadAudioFile(const std::string& filePath) {
        // Create a temporary WAV file
        std::string tempWavFile = "/tmp/temp_audio_" + std::to_string(std::hash<std::string>{}(filePath)) + ".wav";
        std::vector<float> samples;
        
        try {
            // Use FFmpeg to convert to WAV
            std::string command = "ffmpeg -i \"" + filePath + "\" -ac 1 -ar 44100 -f wav \"" + tempWavFile + "\" -y 2>/dev/null";
            
            std::cout << "Executing: " << command << std::endl;
            int result = system(command.c_str());
            
            if (result != 0) {
                std::cerr << "ERROR: Failed to convert file using FFmpeg: " << filePath << std::endl;
                std::cerr << "Make sure FFmpeg is installed and in your PATH." << std::endl;
                return {};
            }
            
            // Load the WAV file
            samples = loadWavFile(tempWavFile);
        } catch (const std::exception& e) {
            std::cerr << "Exception during audio processing: " << e.what() << std::endl;
        }
        
        // Clean up temporary file - always executed even if an exception occurs
        if (std::remove(tempWavFile.c_str()) == 0) {
            std::cout << "Temporary file removed: " << tempWavFile << std::endl;
        }
        
        return samples;
    }

private:
    /**
     * Find a chunk in a WAV file
     * 
     * @param fileData The WAV file binary data
     * @param chunkId The ID of the chunk to find
     * @return The position of the chunk, or -1 if not found
     */
    static int findChunk(const std::vector<uint8_t>& fileData, const std::string& chunkId) {
        size_t fileSize = fileData.size();
        
        // Start after RIFF header
        for (size_t i = 12; i < fileSize - 8; i++) {
            if (i + 4 > fileSize) break;
            
            if (fileData[i] == chunkId[0] &&
                fileData[i + 1] == chunkId[1] &&
                fileData[i + 2] == chunkId[2] &&
                fileData[i + 3] == chunkId[3]) {
                return static_cast<int>(i);
            }
        }
        
        return -1;
    }
    
    /**
     * Read a 16-bit unsigned integer from a byte array
     */
    static uint16_t readUint16(const std::vector<uint8_t>& data, int offset) {
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }
    
    /**
     * Read a 32-bit unsigned integer from a byte array
     */
    static uint32_t readUint32(const std::vector<uint8_t>& data, int offset) {
        return (data[offset] & 0xFF) |
               ((data[offset + 1] & 0xFF) << 8) |
               ((data[offset + 2] & 0xFF) << 16) |
               ((data[offset + 3] & 0xFF) << 24);
    }
};

#endif // AUDIO_READER_H