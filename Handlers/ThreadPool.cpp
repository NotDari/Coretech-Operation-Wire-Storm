#include "ThreadPool.h"
#include "../Clients/DestinationClient.h"

#include "../Utils/Logger.h"

/**
 * The task for the threads to complete.
 * While they have not been told to stop, waits the next destination client to be processed.
 * Then it attempts to send the message, removing the destination client if the pipe is closed
 * or throwing an error if one occurs
 */
void ThreadPool::threadTask() {
    while (!*stop) {
       //log("Thread: " <<  << " searching", LoggerLevel::INFO);

        //Getting and waiting for the client. Logging and returning if error
        std::shared_ptr<DestinationClient> client;
        auto expectedDestinationClient = destinationClientHandler->getDestinationClientFromQueue(stop);
        if (expectedDestinationClient.hasError()) {
            Logger::log(expectedDestinationClient.getError(), expectedDestinationClient.getLoggerLevel());
            continue;
        }
        //Attempting to send the message to the client.
        client = expectedDestinationClient.getValue();
        auto expectedSendMessage = client->sendMessage();
        if (expectedSendMessage.hasError()) {
            //If connection is broken, remove that destination client from the list
            if (expectedSendMessage.getErrorCode() == ErrorCode::BROKEN_PIPE) {
                auto expectedRemoveDestination = destinationClientHandler->removeDestination(expectedDestinationClient.getValue()->getSocketId());
                if (expectedRemoveDestination.hasError()) {
                    Logger::log(expectedRemoveDestination.getError(), expectedRemoveDestination.getLoggerLevel());
                } else {
                    Logger::log("Successfully removed closed destination", LoggerLevel::DEBUG);
                }
            }else {
                Logger::log(expectedSendMessage.getError(), expectedSendMessage.getLoggerLevel());
            }

        }
    }
}

/**
 * The constructor which sets the initials variables and creates the threads, adding them to the list.
 *
 * @param numThreads (uint8_t) - number of threads to have in the pool
 * @param destinationClientHandler  (std::shared_ptr<DestinationClientHandler>) - shared pointer to DestinationClientHandler which allows the access of destination clients
 * @param stop (std::atomic<bool>*) - atomic bool telling the threads when to stop.
 */
ThreadPool::ThreadPool(uint8_t numThreads, std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop) {
    Logger::log("ThreadPool Starting", LoggerLevel::INFO);
    this->destinationClientHandler = destinationClientHandler;
    this->stop = stop;

    //Make threads
    threadList.reserve(numThreads);
    for (uint8_t i = 0; i < numThreads; i++) {
        threadList.emplace_back([this] {this->threadTask();});
    }

}

/**
 * Destructor. Join all the threads so that their lifecycle isn't terminated before the pools,
 * as this caused errors.
 */
ThreadPool::~ThreadPool() {
    for (auto &t : threadList) {
        if (t.joinable()) t.join();
    }
}