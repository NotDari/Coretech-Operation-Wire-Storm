#pragma once

#include <thread>

class ThreadPool {
private:
    std::vector<std::thread> threadList;


    void threadTask();

public:
    ThreadPool(uint8_t numThreads) ;
};
