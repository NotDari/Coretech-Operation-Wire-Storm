#pragma once
#include "DestinationClient.h"

//TODO Change Singleton Design pattern
class DestinationClientHandler {
private:
    // The list of all the Destination Clients. Uses a pointer
    std::unordered_map<int, std::shared_ptr<DestinationClient>> destinationList;
    // A lock to prevent concurrent access of destinationList
    std::mutex destinationListMutex;

    //The queue of Destination Sources which are available
    std::deque<int> queue;



    DestinationClientHandler() {}

    //Stops singleton copying
    DestinationClientHandler(const DestinationClientHandler&) = delete;
    DestinationClientHandler& operator=(const DestinationClientHandler&) = delete;


    static std::mutex singleTonMtx;
    static DestinationClientHandler* instance;

public:

    static DestinationClientHandler* getInstance();

    void addNewDestination(int socketId);


    void removeDestination(int socketId);



    void addMessage(std::shared_ptr<CTMP> message);







};
static std::mutex singleTonMtx;
static DestinationClientHandler* instance;
