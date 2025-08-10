#include "ThreadPool.h"

    void ThreadPool::threadTask() {
        while (true) {

        }
    }

    ThreadPool::ThreadPool(uint8_t numThreads) {
        threadList.reserve(numThreads);
        //TODO since numThreads might be user defined at some point, don't forget to check for error having so many threads
        for (uint8_t i = 0; i < numThreads; i++) {
            threadList.emplace_back([i] () {

            });
        }

    }
