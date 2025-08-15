//
// Created by Dariush Tomlinson on 11/08/2025.
//

#pragma once

#include <string>
#include <chrono>
#include <iostream>


enum LoggerLevel{
    INFO,
    DEBUG,
    WARN,
    ERROR
};

class Logger {
private:
    static std::mutex logMutex;  // Declare mutex



public:

    static bool includeDebug;

    static std::string convertLoggerLevelString(LoggerLevel level);

    static void log(std::string message, LoggerLevel level);


};






