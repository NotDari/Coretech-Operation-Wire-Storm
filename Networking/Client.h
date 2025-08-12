#pragma once



class Client {
    int socketId;


public:
    Client(int socketId) : socketId(socketId){};

    int getSocketId();


    bool operator==(const Client& other) const;
};




