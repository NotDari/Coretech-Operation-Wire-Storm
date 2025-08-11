#pragma once

#include "../CTMP.h"
#include <iostream>
#include <thread>




class DestinationClient {
private:
    int socketId;
    //Queue of Pointers to CTMP Messages
    std::queue<std::shared_ptr<CTMP>> queue;
    //Lock for this class
    std::mutex destinationClientMutex;



public:
    DestinationClient(int socketId);

    void addMessageToQueue(std::shared_ptr<CTMP> message);

    std::shared_ptr<CTMP> accessMessageItem();

    int getSocketId();


    bool operator==(const DestinationClient& other) const;




};

