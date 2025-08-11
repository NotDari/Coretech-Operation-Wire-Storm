#pragma once

#include <thread>

class ThreadPool {
private:
    std::vector<std::thread> threadList;
    std::atomic<bool> stop{false};

    void threadTask();

public:
    ThreadPool(uint8_t numThreads) ;
};
