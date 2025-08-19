//
// Created by Dariush Tomlinson on 12/08/2025.
//

#pragma once
#include <string>
#include <optional>
#include "Logger.h"
#include "ErrorCode.h"

/**
 * A helper class used to error handle, and contain errors/successful values
 *
 * Variables:
 * value - the value which is to be used if there is no error
 * error - the error string if an error occurs
 * level - the LoggerLevel of the error if one occurs for potential outputting
 * errorCode - the ErrorCode of the error if one occurs for potential handling
 *
 * Methods:
 * Expected(T value) - Constructor for successful expected, assigns value
 * Expected(std::string error, LoggerLevel level, ErrorCode errorCode) - Constructor for unsuccessful expected, assigns error
 * hasError - Checks whether this expected contains an error
 * getValue - Gets the value assigned to this class
 * getError - Gets the error assigned to this class
 * getLoggerLevel - Gets the LoggerLevel assigned to this class
 * getErrorCode - Gets the error code assigned to this class
 *
 * @tparam T The type of value stored
 */
template <typename T>
class Expected {
private:
    std::optional<T> value;
    std::string error;
    LoggerLevel level;
    ErrorCode errorCode;

public:
    Expected(T value) : value(std::move(value)){};

    Expected(std::string error, LoggerLevel level, ErrorCode errorCode) : error(std::move(error)), level(level), errorCode(errorCode){};


    bool hasError(){return !error.empty();};

    T& getValue(){return *value;}

    std::string getError() {
        return error;
    }

    LoggerLevel getLoggerLevel() {
        return level;
    }

    ErrorCode getErrorCode() {
        return errorCode;
    }



};


/**
 * A helper class used to error handle, but contain no values
 *
 * Variables:
 * error - the error string if an error occurs
 * level - the LoggerLevel of the error if one occurs for potential outputting
 * errorCode - the ErrorCode of the error if one occurs for potential handling
 *
 * Methods:
 * Expected() - Constructor for successful expected, sets DEBUG level
 * Expected(std::string error, LoggerLevel level, ErrorCode errorCode) - Constructor for unsuccessful expected, assigns error
 * hasError - Checks whether this expected contains an error
 * getError - Gets the error assigned to this class
 * getLoggerLevel - Gets the LoggerLevel assigned to this class
 * getErrorCode - Gets the error code assigned to this class
 *
 * Stores no value so of type void.
 */
template <>
class Expected<void> {
private:
    std::string error;
    LoggerLevel level;
    ErrorCode errorCode;


public:
    Expected() : error(""), level(LoggerLevel::DEBUG){};
    Expected(std::string error, LoggerLevel level, ErrorCode errorCode) : error(std::move(error)), level(level), errorCode(errorCode){};

    bool hasError(){return !error.empty();};


    std::string getError() {
        return error;
    }

    LoggerLevel getLoggerLevel() {
        return level;
    }

    ErrorCode getErrorCode() {
        return errorCode;
    }


};