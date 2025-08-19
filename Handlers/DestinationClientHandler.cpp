#include <iostream>

#include "../Handlers/DestinationClientHandler.h"





/**
 * In order to keep this file thread safe, lock the queue/set mutex first and then the destination map one.
 */


/**
 * Attempts to add a new destination to the destination map, returning an error if it fails.
 * Thread-safe, acquires the map mutex;
 *
 * @param socketId (int) - Make a new destination, andd add it to the destination map
 * @return - Expected containing nothing if no error, the Error if there is one
 */
Expected<void> DestinationClientHandler::addNewDestination(int socketId) {
    std::lock_guard<std::mutex> lock(destinationMapMutex);
    try {
        //Create shared pointer for the destination, and adds it to the map
        std::shared_ptr<DestinationClient> destinationClientPtr = std::make_shared<DestinationClient>(socketId);
        destinationMap.insert({socketId,destinationClientPtr});
    } catch (std::exception& e) {
        return Expected<void>("Failed to add new destination: " + std::string(e.what()), LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }
    return {};
}


/**
 * Attempts to remove a destination from the destination map and the queue, returning an error if it fails.
 * Thread-safe, acquires the queue mutex first and then the map mutex;
 *
 * @param socketId (int) - id of the destination to be removed
 * @return - Expected containing nothing if no error, the Error if there is one
 */
Expected<void> DestinationClientHandler::removeDestination(int socketId) {
    //Attempts to remove Destination from queue and map
    try {
        std::lock_guard lockQueue(queueSetMutex);
        idInQueueSet.erase(socketId);
        queue.erase(std::remove(queue.begin(), queue.end(), socketId), queue.end());

        std::lock_guard<std::mutex> lock(destinationMapMutex);
        destinationMap.erase(socketId);
    } catch (...){
        return Expected<void>("Failed to remove Broken Destination", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }

    return {};
}


/**
 * Gets the next destination whose message is to be processed.
 *
 * First acquires the queue lock, and checks whether the queue is empty or if stop has been called.
 * If the queue is empty and stop isn't called it sleeps until it can try again.
 * If stop has been called, it returns with the error code indicateing a stoppage.
 * If the queue isn't empty it gets the element(id of the next Destination Client) at the front of the queue
 * ,then releases the lock.
 * Afterwards it proceeds to find and return the destinationClient in a thread safe way
 *
 * @return - the expected containing shared pointer of the destination client which is at the front of the queue
 * or the error that occurs.
 */
Expected<std::shared_ptr<DestinationClient>> DestinationClientHandler::getDestinationClientFromQueue(std::atomic<bool>* stop) {
    std::unique_lock lockQueueSetMap(queueSetMutex);

    //Waiting until notified and either stop is called or queue is not empty
    this->conditionVariable.wait(lockQueueSetMap, [this, stop](){ return !queue.empty() || *stop;});


    //Checking if stop is called or queue is empty for error handling
    if (*stop) {
        return Expected<std::shared_ptr<DestinationClient>>("ThreadPool stopping", LoggerLevel::WARN, ErrorCode::STOP_THREAD);
    }
    if (queue.empty()) {
        return Expected<std::shared_ptr<DestinationClient>>("DestinationClientQueue is empty. Error with mutex", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }

    //Getting front element from queue and removing it from set
    int destinationClientId = queue.front();
    queue.pop_front();
    idInQueueSet.erase(destinationClientId);
    lockQueueSetMap.unlock();

    //Getting element from destinationMap
    std::lock_guard lockDestinationMap(destinationMapMutex);
    auto mapElement = destinationMap.find(destinationClientId);
    if (mapElement == destinationMap.end()) {
        return Expected<std::shared_ptr<DestinationClient>>("Queue Destination doesn't exist anymore", LoggerLevel::WARN, ErrorCode::DEFAULT);
    }
    if (!mapElement->second) {
        return Expected<std::shared_ptr<DestinationClient>>("Queue Destination is null", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }

    return mapElement->second;
}

/**
 * Adds a message to every destination currently held.
 *
 * This updated addMessage handles locks efficiently, to attempt to reduce thread waiting.
 * It first acquires the destinationMap lock to get temporary pointers to all the destinations before releasing the lock.
 * It then loops through, adding the message to every destination.
 * It then acquires the queue/set mutex and adds all items which aren't already in the queue to the queue.
 *
 *
 * @param message (shared_ptr<CTMP>) Message to be sent to all the clients.
 * @return - Expected containing nothing unless an error occurs, in which case the error will be returned
 */
Expected<void> DestinationClientHandler::addMessage(std::shared_ptr<CTMP> message) {
    //Error handling for if message is null
    if (!message) {
        return Expected<void>("Message to be added is null", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }
    //Making temp list of Destination clients to be copied into
    std::vector<std::shared_ptr<DestinationClient>> tempDestinationList;

    //Getting all destinations in a thread safe way then releasing the lock
    std::unique_lock lock(destinationMapMutex);
    if (destinationMap.empty()) {
        return Expected<void>("No messages to add to", LoggerLevel::WARN, ErrorCode::DEFAULT);
    }
    tempDestinationList.reserve(destinationMap.size());
    for (auto& entry : destinationMap) {
        tempDestinationList.push_back(entry.second);
    }
    lock.unlock();

    // Adding the message to each destination
    for (std::shared_ptr destinationClient : tempDestinationList){
        destinationClient -> addMessageToQueue(message);
    }

    //Adding each destination which isn't in the queue, into the queue
    std::unique_lock lockQueueSetMap(queueSetMutex);
    for (std::shared_ptr destinationClient : tempDestinationList){
        int socketId = destinationClient->getSocketId();
        if (!this->idInQueueSet.contains(socketId)) {
            queue.push_back(socketId);
            idInQueueSet.insert(socketId);
        }
    }
    //Waking all threads up
    conditionVariable.notify_all();
    return {};
}


/**
 * Attempts to notify all clients, returning an error upon failure
 *
 * @return - Expected containing nothing unless an error occurs, in which case the error will be returned
 */
Expected<void> DestinationClientHandler::notifyAll() {
    try {
        this->conditionVariable.notify_all();
        return {};
    } catch (...) {
        return Expected<void>("Failed to notify All", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }
}





