#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "../CTMP.h"
#include "DestinationClient.h"
#include "DestinationClientHandler.h"
#include <condition_variable>


std::mutex DestinationClientHandler::singleTonMtx;
DestinationClientHandler* DestinationClientHandler::instance = nullptr;

DestinationClientHandler* DestinationClientHandler::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(singleTonMtx);
        if (instance == nullptr) {
            instance = new DestinationClientHandler();
        }
    }
    return instance;
}

//Called when a new destination client is added.
//Adds it to the hashmap
void DestinationClientHandler::addNewDestination(int socketId) {
    std::lock_guard<std::mutex> lock(destinationMapMutex);
    std::shared_ptr<DestinationClient> destinationClientPtr = std::make_shared<DestinationClient>(socketId);
    destinationMap.insert({socketId,destinationClientPtr});
}


//Removes a destination client from the hashmap
void DestinationClientHandler::removeDestination(int socketId) {
    std::lock_guard<std::mutex> lock(destinationMapMutex);
    destinationMap.erase(socketId);
}

/**
 *
 * @return
 */
std::shared_ptr<DestinationClient> DestinationClientHandler::getDestinationClientFromQueue() {
    std::unique_lock<std::mutex> lock(queueSetMutex);
    /**
     * LONG MESSAGE TO REMEMBER LOGIC BEHIND CONDITION VARIABLES
     * The way the logic behind this works is: You have a mutex for the queue and its associated hashset.
     * When a thread tries to access a variable, it has to acquire its lock first. Once it does, it checks
     * if the condition variable's check is true(the queue isn't empty). If it isn't, it goes to sleep and releases the lock
     * and waits to be notified to try again. If the notify isn't linked perfectly, you could have an issue where the queue is
     * not empty and the thread never wakes up. The queue has to have the lock to check the check.
     * Once it gets the lock, it goes to sleep if the thread isn't empty.
     *
     */
    this->conditionVariable.wait(lock, [this](){!this->queue.empty();});

    int destinationClientId = queue.front();
    queue.pop_front();
    idInQueueSet.erase(destinationClientId);
    auto mapElement = destinationMap.find(destinationClientId);
    if (mapElement == destinationMap.end()) {
        //ERROR, Index doesn't exist;
    }

    return mapElement->second;
}

//Adds a message to all of the destination clients
//Uses a Shared pointer to stop resource copying.
//TODO This seems inefficient, as we lock everything while adding messages.so ressearch alternative
void DestinationClientHandler::addMessage(std::shared_ptr<CTMP> message) {
    std::lock_guard<std::mutex> lock(destinationMapMutex);
    for (auto& entry : destinationMap) {
        entry.second -> addMessageToQueue(message);
        int socketId = entry.second->getSocketId();
        std::lock_guard<std::mutex> lock(queueSetMutex);
        if (!this->idInQueueSet.contains(socketId)) {
            queue.push_back(socketId);
            idInQueueSet.insert(socketId);
        }
    }
    conditionVariable.notify_all();
}








