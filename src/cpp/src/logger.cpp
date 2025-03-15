#include "../include/logger.hpp"
#include <iostream>

namespace sortify {
namespace audio {

// Initialize static members of Logger
LogLevel Logger::currentLogLevel = LogLevel::ERROR;
std::function<void(LogLevel, const std::string&)> Logger::logFunction = nullptr;

void Logger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void Logger::setLogFunction(std::function<void(LogLevel, const std::string&)> logFunc) {
    logFunction = std::move(logFunc);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    // Skip messages with level lower than the current log level
    if (level < currentLogLevel) {
        return;
    }
    
    // If a custom log function is set, use it
    if (logFunction) {
        logFunction(level, message);
        return;
    }
    
    // Default implementation: log to standard output/error
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG:   levelStr = "DEBUG"; break;
        case LogLevel::INFO:    levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::ERROR:   levelStr = "ERROR"; break;
        case LogLevel::FATAL:   levelStr = "FATAL"; break;
    }
    
    if (level >= LogLevel::ERROR) {
        std::cerr << "[" << levelStr << "] " << message << std::endl;
    } else {
        std::cout << "[" << levelStr << "] " << message << std::endl;
    }
}

} // namespace audio
} // namespace sortify