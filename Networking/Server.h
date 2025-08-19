#pragma once
#include "../Utils/Expected.h"

/**
 * This class monitors the complete lifecycle of a "Server",
 * which controls the opening and closing of sockets,
 * as well as the binding, listening to and accepting clients.
 *
 * Variables:
 * serverSocket - Id of the server socket
 * portNumber - Port to bind the server to
 * listenNumber - Max number of active connections to be in the listen queue
 *
 * Methods:
 * Server - Constructor, assigns port and listen numbers and assigns temporary false socket
 * ~Server - Destructor, calls stop() to close the sockets
 * initiateProtocol - Creates bind and listens to server socket
 * initiateClient - Accepts a client connection request
 * stop - Closes the server socket if in use
 */
class Server {
    int serverSocket;
    uint16_t portNumber;
    int listenNumber;

    Expected<int> createSocket();

    Expected<void> bindSocket();

    Expected<void> listenOnSocket();

public:

    Server(uint16_t portNumber, uint16_t listenNumber): portNumber(portNumber),serverSocket(-1), listenNumber(listenNumber){};

    ~Server();

    Expected<int> initiateProtocol();

    Expected<int> initiateClient();

    void stop();
};
