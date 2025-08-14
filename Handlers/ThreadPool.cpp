#include "ThreadPool.h"
#include "../Networking/Clients/DestinationClient.h"

#include "../Utils/Logger.h"

void ThreadPool::threadTask() {
        while (!stop) {
           //log("Thread: " <<  << " searching", LoggerLevel::INFO);
            std::shared_ptr<DestinationClient> client;
            auto expectedDestinationClient = destinationClientHandler->getDestinationClientFromQueue();
            if (expectedDestinationClient.hasError()) {
                Logger::log(expectedDestinationClient.getError(), expectedDestinationClient.getLoggerLevel());
                continue;
            }
            client = expectedDestinationClient.getValue();
            auto expectedSendMessage = client->sendMessage();
            if (expectedSendMessage.hasError()) {
                if (expectedSendMessage.getErrorCode() == ErrorCode::BrokenPipe) {
                    auto expectedRemoveDestination = destinationClientHandler->removeDestination(expectedDestinationClient.getValue()->getSocketId());
                    if (expectedRemoveDestination.hasError()) {
                        Logger::log(expectedRemoveDestination.getError(), expectedRemoveDestination.getLoggerLevel());
                    } else {
                        Logger::log("Successfully removed closed destination", LoggerLevel::INFO);
                    }
                }else {
                    Logger::log(expectedSendMessage.getError(), expectedSendMessage.getLoggerLevel());
                }

            }

        }
    }

    ThreadPool::ThreadPool(uint8_t numThreads, std::shared_ptr<DestinationClientHandler> destinationClientHandler) {
        Logger::log("ThreadPool Starting", LoggerLevel::INFO);
        this->destinationClientHandler = destinationClientHandler;
        threadList.reserve(numThreads);
        //TODO since numThreads might be user defined at some point, don't forget to check for error having so many threads
        for (uint8_t i = 0; i < numThreads; i++) {
            threadList.emplace_back([this] {this->threadTask();});
        }

    }
