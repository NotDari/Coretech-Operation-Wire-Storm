#pragma once

#include "../Networking/Protocols/CTMP.h"
#include "Client.h"
#include "../Utils/Expected.h"
#include <thread>
#include <queue>


/**
 * An implementation of the base Client Class.
 * Represents a Destination Client and handles the ordering and sending of messages.
 *
 * Variables:
 * queue - a queue of shared pointers to the next messages to be processed
 * destinationClientMutex - lock for the queue, and for this class
 *
 * Methods:
 * accessMessageItem - Get the next message in the queue
 * DestinationClient - Constructor, creating a new base client, setting socketId
 * addMessageToQueue - adds a message to this class' queue
 * sendMessage - sends a message to the destination client
 */
class DestinationClient : public Client {
private:

    //Queue of Pointers to CTMP Messages
    std::queue<std::shared_ptr<CTMP>> queue;
    //Lock for this class
    std::mutex destinationClientMutex;

    Expected<std::shared_ptr<CTMP>> accessMessageItem();


public:

    DestinationClient(int socketId) : Client(socketId){};

    void addMessageToQueue(std::shared_ptr<CTMP> message);

    Expected<void> sendMessage();

};

