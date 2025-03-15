#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <functional>

namespace sortify {
namespace audio {

/**
 * @enum LogLevel
 * @brief Defines the logging severity levels
 */
enum class LogLevel {
    DEBUG,   ///< Detailed information, typically only valuable when diagnosing problems
    INFO,    ///< Confirmation that things are working as expected
    WARNING, ///< Indication that something unexpected happened, but still working
    ERROR,   ///< Due to a more serious problem, some functionality is impaired
    FATAL    ///< Very severe error that will likely lead to application termination
};

/**
 * @class Logger
 * @brief Provides logging functionality for the audio fingerprinting system
 *
 * This class provides static methods to log messages at various severity levels.
 * The default implementation writes to standard output/error, but a custom log
 * function can be set to redirect logs elsewhere.
 */
class Logger {
public:
    /**
     * Sets the current log level. Messages with lower severity than this level will be ignored.
     *
     * @param level The minimum log level to output
     */
    static void setLogLevel(LogLevel level);
    
    /**
     * Sets a custom log function to handle log messages.
     *
     * @param logFunc Function that takes a LogLevel and a message string
     */
    static void setLogFunction(std::function<void(LogLevel, const std::string&)> logFunc);
    
    /**
     * Log a debug message
     *
     * @param message The message to log
     */
    static void debug(const std::string& message);
    
    /**
     * Log an info message
     *
     * @param message The message to log
     */
    static void info(const std::string& message);
    
    /**
     * Log a warning message
     *
     * @param message The message to log
     */
    static void warning(const std::string& message);
    
    /**
     * Log an error message
     *
     * @param message The message to log
     */
    static void error(const std::string& message);
    
    /**
     * Log a fatal message
     *
     * @param message The message to log
     */
    static void fatal(const std::string& message);

private:
    /**
     * Log a message with the specified level
     *
     * @param level The severity level of the message
     * @param message The message to log
     */
    static void log(LogLevel level, const std::string& message);

    // Static members
    static LogLevel currentLogLevel;
    static std::function<void(LogLevel, const std::string&)> logFunction;
};

} // namespace audio
} // namespace sortify

#endif // LOGGER_HPP