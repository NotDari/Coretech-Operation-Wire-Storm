#pragma once
#include <unistd.h>


class Client {
    int socketId;


public:

    ~Client();

    Client(int socketId) : socketId(socketId){};

    int getSocketId();

    void closeClient();
    bool operator==(const Client& other) const;
};




