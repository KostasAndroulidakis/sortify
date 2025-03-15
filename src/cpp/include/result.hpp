#ifndef RESULT_HPP
#define RESULT_HPP

#include <string>
#include <optional>

namespace sortify {
namespace audio {

/**
 * @class Result
 * @brief A template class for returning operation results with error handling
 * 
 * This class encapsulates the result of an operation that might fail.
 * It contains a success flag, an optional error message, and an optional value.
 * 
 * @tparam T The type of the value contained in a successful result
 */
template<typename T>
class Result {
public:
    /**
     * Creates a successful result with a value
     * 
     * @param val The operation result value
     * @return A successful Result object containing the value
     */
    static Result<T> createSuccess(T val) {
        Result<T> result;
        result.success = true;
        result.value = std::move(val);
        return result;
    }
    
    /**
     * Creates a failed result with an error message
     * 
     * @param message The error message
     * @return A failed Result object containing the error message
     */
    static Result<T> createFailure(std::string message) {
        Result<T> result;
        result.success = false;
        result.errorMessage = std::move(message);
        return result;
    }
    
    /**
     * Check if the operation was successful
     * 
     * @return true if the operation succeeded, false otherwise
     */
    bool isSuccess() const {
        return success;
    }
    
    /**
     * Get the result value
     * Only valid if isSuccess() returns true
     * 
     * @return The operation result value
     */
    const T& getValue() const {
        return value.value();
    }
    
    /**
     * Get the error message
     * Only valid if isSuccess() returns false
     * 
     * @return The error message
     */
    const std::string& getError() const {
        return errorMessage;
    }
    
    // Public members for easier access in test code
    bool success = false;
    std::optional<T> value;
    std::string errorMessage;
};

} // namespace audio
} // namespace sortify

#endif // RESULT_HPP