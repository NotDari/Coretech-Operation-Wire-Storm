#include "Server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>




Server::~Server() {
    stop();
}

Expected<int> Server::createSocket() {
    //Creating the server socket
    int socketAssignment = socket(AF_INET, SOCK_STREAM, 0);
    if (socketAssignment == -1) {
        return {"Issues Creating socket:" + std::string(strerror(errno))};
    }
    return {socketAssignment};
}

Expected<int> Server::bindSocket() {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int out = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (out < 0) {
        return {"Failed to bind Socket on port:" + std::to_string(this->portNumber) + " ,with err:" + std::string(strerror(errno))};
    }
    return {out};
};

Expected<int> Server::listenOnSocket() {
    if (listen(serverSocket, 1) < 0) {
        return {"Failed to listen on socket: " + std::string(strerror(errno))};
    }
    return {serverSocket};
};

Expected<int> Server::initiateProtocol() {
    auto expectedSocket = createSocket();
    if (expectedSocket.hasError()) {
        return expectedSocket;
    }
    this->serverSocket = expectedSocket.getValue();

    auto expectedBindSocket = bindSocket();
    if (expectedBindSocket.hasError()) {
        return expectedBindSocket;
    }

    auto expectedListenSocket = listenOnSocket();
    if (expectedListenSocket.hasError()) {
        return expectedListenSocket;
    }

    return expectedSocket.getValue();



}

Expected<int> Server::initiateClient() {

}


void Server::stop() {
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
}
