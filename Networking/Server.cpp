#include "Server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



Server::~Server() {
    stop();
}

/**
 * Attempts to create the server socket using pre-set IPV4 format, throwing an error if unsuccessful.
 * This is the first step for accepting clients.
 *
 * @return (Expected<int>) serverSocked Id in expected if successful/ error within expected if not
 */
Expected<int> Server::createSocket() {
    int socketAssignment = socket(AF_INET, SOCK_STREAM, 0);
    if (socketAssignment == -1) {
        return { std::string("Issues Creating socket: ") + strerror(errno), LoggerLevel::ERROR, ErrorCode::Default};
    }
    return {socketAssignment};
}

/**
 * This functions attempts to bind the server socket to the previously configured port.
 * It returns an error within Expected if not successful.
 * This must be done in order to listen for clients.
 *
 * @return (Expected<void>) void in expected if successful/ error within expected if not
 */
Expected<void> Server::bindSocket() {
    //Setting the port, address type(e.g.ipv4) and available address
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int bindOutcome = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (bindOutcome < 0) {
        return {"Failed to bind Socket on port:" + std::to_string(this->portNumber) + " ,with err:" + std::string(strerror(errno)), LoggerLevel::ERROR , ErrorCode::Default};
    }
    return {};
};

/**
 * This function attempts to listen to the socket. It returns an error within Expected if not successful.
 * It allows a set number(listenNumber) of clients to be waiting to be accepted.
 *
 * @return (Expected<void>) void in expected if successful/ error within expected if not
 */
Expected<void> Server::listenOnSocket() {
    if (listen(serverSocket, listenNumber) < 0) {
        return {"Failed to listen on socket: " + std::string(strerror(errno)), LoggerLevel::ERROR, ErrorCode::Default };
    }
    return {};
};

/**
 * This function initiates the protocol by calling all the sub functions.
 * It calls the function to create the socket. It alls the function to bind the socket.It calls the function to listen to the socket.
 * If there is an error throughout any of these sub functions, it is returned within the expected.
 *
 * @return (Expected<int>) - The Socket ID of the Server if successful/ The Error if not (both within expected)
 */
Expected<int> Server::initiateProtocol() {
    //Creating the socket
    auto expectedSocket = createSocket();
    if (expectedSocket.hasError()) {
        return expectedSocket;
    }
    this->serverSocket = expectedSocket.getValue();


    //Binding the Socket
    auto expectedBindSocket = bindSocket();
    if (expectedBindSocket.hasError()) {
        stop();
        return {expectedBindSocket.getError(), expectedBindSocket.getLoggerLevel(), expectedBindSocket.getErrorCode()};
    }

    //Listening to the Socket
    auto expectedListenSocket = listenOnSocket();
    if (expectedListenSocket.hasError()) {
        stop();
        return {expectedListenSocket.getError(), expectedListenSocket.getLoggerLevel(), expectedListenSocket.getErrorCode()};
    }

    return expectedSocket.getValue();



}

/**
 * This attempts to initialise a A based on the server's socket id.
 * This means accepting a client's connection request, so that message transmitting can begin.
 * It returns an Error in expected if something goes wrong.
 *
 * @return (Expected<int>) - the socketId of the accepted client / an error
 */
Expected<int> Server::initiateClient() {
    if (serverSocket == -1) {
        return {"Trying to initialise client on closed socket", LoggerLevel::ERROR, ErrorCode::Default };
    }
    //Waiting for a client to request connection so they can be accepted
    sockaddr_in ClientAddress;
    socklen_t sourceAddressLength = sizeof(ClientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &ClientAddress, &sourceAddressLength);
    if (clientSocket < 0) {
        return {"Failed to initialise client: " + std::string(strerror(errno)), LoggerLevel::ERROR , ErrorCode::Default};
    }
    Logger::log("Accepted Client", LoggerLevel::INFO);
    return {clientSocket};
}


/**
 * This resets the Server and closes the socket, if its been opened
 */
void Server::stop() {
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
}
