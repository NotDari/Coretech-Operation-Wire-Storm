//
// Created by Dariush Tomlinson on 12/08/2025.
//

#pragma once
#include <string>
#include <optional>
#include "Logger.h"
#include "ErrorCode.h"

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