//
// Created by Dariush Tomlinson on 12/08/2025.
//

#pragma once
#include <string>
#include <optional>
#include "Logger.h"

template <typename T>
class Expected {
private:
    std::optional<T> value;
    std::string error;
    LoggerLevel level;

public:
    Expected(T value) : value(std::move(value)){};

    Expected(std::string error, LoggerLevel level) : error(std::move(error)), level(level){};

    bool hasError(){return !error.empty();};

    T& getValue(){return *value;}

    std::string getError() {
        return error;
    }

    LoggerLevel getLoggerLevel() {
        return level;
    }


};