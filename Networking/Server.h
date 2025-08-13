#pragma once
#include "../Utils/Expected.h"

/**
 * This class monitors the complete lifecycle of a "Server",
 * which controls the opening and closing of sockets,
 * as well as the binding, listening to and accepting clients.
 */
class Server {
    //Id of the server socket
    int serverSocket;
    //Port to bind the server to
    uint16_t portNumber;
    //Max number of active connections to be in the listen queue
    int listenNumber;

    Expected<int> createSocket();

    Expected<void> bindSocket();

    Expected<void> listenOnSocket();

public:

    Server(uint16_t portNumber, uint16_t listenNumber): portNumber(portNumber),serverSocket(-1), listenNumber(listenNumber){};


    ~Server();

    //Creates bind and listens to server socket
    Expected<int> initiateProtocol();

    //Accepts a client connection request
    Expected<int> initiateClient();

    //Closes the server socket if in use
    void stop();
};
