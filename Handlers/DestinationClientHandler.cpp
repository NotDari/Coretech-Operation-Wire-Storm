#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "../CTMP.h"
#include "../Networking/Clients/DestinationClient.h"
#include "../Handlers/DestinationClientHandler.h"
#include <condition_variable>


/**
 * In order to keep this file thread safe, lock the queue/set mutex first and then the destination map one.
 */

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
 * Gets the next destination whose message is to be processed.
 *
 * First acquires the queue lock, and checks whether the queue is empty. If not proceeds, else it
 * waits to be notified to try again.
 * It gets the element(id of the next Destination Client) at the front of the queue,then releases the lock.
 * Afterwards it proceeds to find and return the destionationClient in a thread safe way
 *
 * @return the destination client which is at the front of the queue.
 */
std::shared_ptr<DestinationClient> DestinationClientHandler::getDestinationClientFromQueue() {
    std::unique_lock lockQueueSetMap(queueSetMutex);
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

    std::cout << "Waiting for message" << std::endl;
    this->conditionVariable.wait(lockQueueSetMap, [this](){ return !this->queue.empty();});

    //Getting front element from queue and removing it from set
    int destinationClientId = queue.front();
    queue.pop_front();
    idInQueueSet.erase(destinationClientId);
    lockQueueSetMap.unlock();

    //Getting element from destinationMap
    std::lock_guard lockDestinationMap(destinationMapMutex);
    auto mapElement = destinationMap.find(destinationClientId);
    if (mapElement == destinationMap.end()) {
        //ERROR, Index doesn't exist;
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
 * TODO Test whether its faster to combine the two loops. It probably should be as addMessageToQueue should be quick
 * TODO could it be faster to hold a temporary queue of destination clients pointers??
 * @param message (shared_ptr<CTMP>) Message to be sent to all the clients.
 */
void DestinationClientHandler::addMessage(std::shared_ptr<CTMP> message) {
    //Making temp list of Destination clients to be copied into
    std::vector<std::shared_ptr<DestinationClient>> tempDestinationList;
    tempDestinationList.reserve(destinationMap.size());

    //Getting all destinations in a thread safe way then releasing the lock
    std::unique_lock lock(destinationMapMutex);
    for (auto& entry : destinationMap) {
        tempDestinationList.push_back(entry.second);
    }
    lock.unlock();

    // Adding the message to each destination
    std::cout << "Adding message" << std::endl;
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
}








