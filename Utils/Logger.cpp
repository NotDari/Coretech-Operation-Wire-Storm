//
// Created by Dariush Tomlinson on 12/08/2025.
//
#include "Logger.h"

std::mutex Logger::logMutex;
bool Logger::includeDebug = false;


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


void Logger::log(std::string message, LoggerLevel level){
    if (!includeDebug && level == LoggerLevel::DEBUG) {
        return;
    }
    std::lock_guard<std::mutex> guard(logMutex);
    auto time = std::chrono::system_clock::now();
    std::cout << "[" << convertLoggerLevelString(level) << "] " << time << " : " << message << std::endl;
}