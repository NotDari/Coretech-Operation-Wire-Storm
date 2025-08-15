//
// Created by Dariush Tomlinson on 11/08/2025.
//

#include "Client.h"
#include <string>
#include "../../Utils/Logger.h"



Client::~Client() {
    closeClient();
}


//Doesn't need to be locked as sockedId is a constant;
int Client::getSocketId() {
    return this->socketId;
}


void Client::closeClient() {
    if (socketId != -1) {
        close(socketId);
        socketId = - 1;
    }
}


//MAY NOT BE NECESSARY AS CONSIDERING USING HASHMAP
//Overriding the equals so when we want to check if two clients are equal, we can.
//This will allow us to remove it.
bool Client::operator==(const Client& other) const {
    return (this->socketId == other.socketId);
}