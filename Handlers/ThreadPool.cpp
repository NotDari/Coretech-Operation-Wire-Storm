#include "ThreadPool.h"
#include "../Networking/Clients/DestinationClient.h"

void ThreadPool::threadTask() {
        std::cout << "In thread" << std::endl;
        while (!stop) {
            std::shared_ptr<DestinationClient> client;

            client = destinationClientHandler->getDestinationClientFromQueue();
            std::cout << "Sending message" << std::endl;
            client->sendMessage();
        }
    }

    ThreadPool::ThreadPool(uint8_t numThreads, std::shared_ptr<DestinationClientHandler> destinationClientHandler) {
        this->destinationClientHandler = destinationClientHandler;
        threadList.reserve(numThreads);
        //TODO since numThreads might be user defined at some point, don't forget to check for error having so many threads
        for (uint8_t i = 0; i < numThreads; i++) {
            threadList.emplace_back([this] {this->threadTask();});
        }

    }
