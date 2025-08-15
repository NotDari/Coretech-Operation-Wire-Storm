#pragma once
#include <unistd.h>


class Client {
    int socketId;
protected:
    Client(int socketId) : socketId(socketId){};

public:

    ~Client();



    int getSocketId();

    void closeClient();
    bool operator==(const Client& other) const;
};




