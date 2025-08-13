#pragma once

#include "../../CTMP.h"
#include "Client.h"
#include "../../Utils/Expected.h"
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



    Expected<std::shared_ptr<CTMP>> accessMessageItem();



    Expected<void> sendMessage();



    void addMessageToQueue(std::shared_ptr<CTMP> message);


};

