#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "../CTMP.h"
#include "DestinationClient.h"
#include "DestinationClientHandler.h"

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
    std::lock_guard<std::mutex> lock(destinationListMutex);
    std::shared_ptr<DestinationClient> destinationClientPtr = std::make_shared<DestinationClient>(socketId);
    destinationList.insert({socketId,destinationClientPtr});
}


//Removes a destination client from the hashmap
void DestinationClientHandler::removeDestination(int socketId) {
    std::lock_guard<std::mutex> lock(destinationListMutex);
    destinationList.erase(socketId);
}


//Adds a message to all of the destination clients
//Uses a Shared pointer to stop resource copying.
//TODO This seems inefficient, as we lock everything while adding messages.so ressearch alternative
void DestinationClientHandler::addMessage(std::shared_ptr<CTMP> message) {
    std::lock_guard<std::mutex> lock(destinationListMutex);
    for (auto& entry : destinationList) {
        entry.second -> addMessageToQueue(message);
    }
}








