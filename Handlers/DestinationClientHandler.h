#pragma once


#include <unordered_set>
#include "../Networking/DestinationClient.h"

//TODO Change Singleton Design pattern
class DestinationClientHandler {
private:
    // The list of all the Destination Clients. Uses a pointer
    std::unordered_map<int, std::shared_ptr<DestinationClient>> destinationMap;
    // A lock to prevent concurrent access of destinationList
    std::mutex destinationMapMutex;

    //The queue of Destination Sources which are available
    std::deque<int> queue;

    std::unordered_set<int> idInQueueSet;
    std::mutex queueSetMutex;

    std::condition_variable conditionVariable;






public:

    DestinationClientHandler() {}

    void addNewDestination(int socketId);


    void removeDestination(int socketId);



    void addMessage(std::shared_ptr<CTMP> message);

    std::shared_ptr<DestinationClient> getDestinationClientFromQueue();






};

