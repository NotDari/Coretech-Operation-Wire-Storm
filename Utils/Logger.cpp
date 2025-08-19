//
// Created by Dariush Tomlinson on 12/08/2025.
//
#include "Logger.h"

std::mutex Logger::logMutex;
bool Logger::includeDebug = false;


/**
 * Converts a LoggerLevel enum member into a string.
 * Defaults to "ERROR";
 *
 * @param level (LoggerLevel) - the level to be converted into a string.
 * @return string for the specified LoggerLevel
 */
std::string Logger::convertLoggerLevelString(LoggerLevel level){
    switch (level) {
        case LoggerLevel::INFO:
            return "INFO";
        case LoggerLevel::DEBUG:
            return "DEBUG";
        case LoggerLevel::WARN:
            return "WARN";
        default: return "ERROR";
    }
}

/**
 * Outputs a log to the console, in a thread safe way.
 * Only outputs debug statements if debug mode is enabled.
 *
 * @param message (std::string) message to be logged
 * @param level (LoggerLevel) level to be logged.
 */
void Logger::log(std::string message, LoggerLevel level){
    //Checking if debug mode is enabled.
    if (!includeDebug && level == LoggerLevel::DEBUG) {
        return;
    }
    //Acquiring lock and printing log to console.
    std::lock_guard<std::mutex> guard(logMutex);
    auto time = std::chrono::system_clock::now();
    std::cout << "[" << convertLoggerLevelString(level) << "] " << time << " : " << message << std::endl;
}