#pragma once


#include <unordered_set>
#include "../Clients/DestinationClient.h"
#include "../Utils/Expected.h"
#include <unordered_map>
#include <deque>
#include <condition_variable>
#include <mutex>


/**
 * Class which acts as a container for all the destination clients.
 * Holds messages added to each of them, the adding/removal of destination clients and
 * the order of which destinationn clients to act on next.
 *
 * In order to keep this file thread safe, if both needed to be held at once,
 * lock the queue/set mutex first and then the destination map one.
 *
 * Variables:
 * destinationMap -  The list of all the Destination Clients. Uses a shared pointer.
 * destinationMapMutex -  A lock to prevent concurrent access of destinationList
 * queue - The queue of Destination Sources which are available
 * idInQueueSet - A set which contains a list of ids in the queue to stop repeating items.
 * queueSetMutex - A lock for the queue to prevent concurrent access
 * conditionVariable - A condition variable to notify the threads to wake up
 *
 *
 * Methods:
 * DestinationClientHandler - Default constructor
 * addNewDestination - Adds a new destination to the map
 * removeDestination - removes a destination from the map
 * getDestinationClientFromQueue - Gets the next destination whose client is to be processed. Waits until one ready
 * addMessage - Adds a message to every destination client
 * notifyAll - Notify all threads to wake up with the condition variable
 */
class DestinationClientHandler {
private:

    std::unordered_map<int, std::shared_ptr<DestinationClient>> destinationMap;

    std::mutex destinationMapMutex;

    std::deque<int> queue;

    std::unordered_set<int> idInQueueSet;
    std::mutex queueSetMutex;

    std::condition_variable conditionVariable;






public:

    DestinationClientHandler() {}

    Expected<void> addNewDestination(int socketId);


    Expected<void> removeDestination(int socketId);


    Expected<std::shared_ptr<DestinationClient>> getDestinationClientFromQueue(std::atomic<bool>& stop);


    Expected<void> addMessage(std::shared_ptr<CTMP> message);

    Expected<void> notifyAll();







};

