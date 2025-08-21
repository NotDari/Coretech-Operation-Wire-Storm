#pragma once

#include <thread>
#include "DestinationClientHandler.h"

/**
 * Class which handles the thread pool.
 *
 * Variables:
 * threadList - vector containing  current threads
 * stop - atomic variables which tells the threads to stop
 * destinationClientHandler - a shared pointer to the handler, when specific tasks need to be performed.
 *
 * Methods:
 * threadTask - the task each thread needs to complete, getting the destination client and sending the message
 * ThreadPool - the constructor, sets variables and creates all the threads
 * ~ThreadPool - the destructor, to ensure thread safety, joins the threads to stop this class being destroyed before them
 *
 */
class ThreadPool {
private:
    std::vector<std::thread> threadList;
    std::atomic<bool>& stop;
    std::shared_ptr<DestinationClientHandler> destinationClientHandler;

    void threadTask();

public:

    ThreadPool(uint8_t numThreads, std::shared_ptr<DestinationClientHandler> handler, std::atomic<bool>& stop) ;

    ~ThreadPool();
};
