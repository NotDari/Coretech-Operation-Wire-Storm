#pragma once
#include <string>
#include <netinet/in.h>  // sockaddr_in etc.
#include <vector>
#include <memory>
#include <optional>

#include "../Utils/Expected.h"


class Server {
    int serverSocket;
    uint16_t portNumber;

    Expected<int> createSocket();

    Expected<int> bindSocket();

    Expected<int> listenOnSocket();

public:

    Server(uint16_t portNumber): portNumber(portNumber),serverSocket(-1){};


    ~Server();

    Expected<int> initiateProtocol();

    Expected<int> initiateClient();


    void stop();
};
