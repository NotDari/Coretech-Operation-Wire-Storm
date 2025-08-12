#pragma once

#include "../../CTMP.h"
#include "Client.h"
#include <iostream>
#include <thread>




class DestinationClient : public Client {
private:

    //Queue of Pointers to CTMP Messages
    std::queue<std::shared_ptr<CTMP>> queue;
    //Lock for this class
    std::mutex destinationClientMutex;


public:


    DestinationClient(int socketId) : Client(socketId){};



    std::shared_ptr<CTMP> accessMessageItem();



    void sendMessage();



    void addMessageToQueue(std::shared_ptr<CTMP> message);


};

