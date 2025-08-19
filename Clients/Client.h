#pragma once

#include <unistd.h>

/**
 * Base Class for the client, which will be used for other clients to implement.
 *
 *
 * Variables:
 * socketId - Socket Id of the Client
 *
 * Methods:
 * Client - Constructor, is protected to stop Client being created as a default(should be a subclass)
 * ~Client - Destructor, calls closeClient()
 * getSocketId - returns the id of the socket
 * closeClient - closes the socket, if its used.
 */
class Client {
    int socketId;
protected:
    Client(int socketId) : socketId(socketId){};

public:

    ~Client(){closeClient();}

    int getSocketId(){return this->socketId;}

    void closeClient() {
        if (socketId != -1) {
            close(socketId);
            socketId = - 1;
        }
    }
};




