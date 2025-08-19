//
// Created by Dariush Tomlinson on 11/08/2025.
//

#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <mutex>



/**
 * Enum which is used for managing the levels of an output log.
 * Explaining the criticality of the situation to the user.
 *
 * Members:
 * INFO - Used for simple statements where there is no concern at all.
 * DEBUG - Only outputted during debug mode, for developer use.
 * WARN - Used when there is a potential error, but it is not critical to the program's execution
 * ERROR - Used when there is a critical error that could crash/harm the program.
 */
enum LoggerLevel{
    INFO,
    DEBUG,
    WARN,
    ERROR
};

/**
 * Class which allows the program to output details to the console in a thread safe way.
 *
 * Variables:
 * logMutex - lock for logging, to ensure thread safety
 * includeDebug - static variable used to determine whether Debug logs are outputted.
 *
 * Methods:
 * convertLoggerLevelString - Given a LoggerLevel, returns an equivalent string for logging
 * log - Logs the message and level into the console, in a thread safe way.
 *
 */
class Logger {
private:
    static std::mutex logMutex;  // Declare mutex

    static std::string convertLoggerLevelString(LoggerLevel level);

public:

    static bool includeDebug;

    static void log(std::string message, LoggerLevel level);

};






