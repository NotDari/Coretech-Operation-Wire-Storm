#pragma once

#include <thread>
#include "DestinationClientHandler.h"

class ThreadPool {
private:
    std::vector<std::thread> threadList;
    std::atomic<bool>* stop;
    std::shared_ptr<DestinationClientHandler> destinationClientHandler;

    void threadTask();

public:
    ~ThreadPool();

    ThreadPool(uint8_t numThreads, std::shared_ptr<DestinationClientHandler> handler, std::atomic<bool>* stop) ;
};
