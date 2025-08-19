//
// Created by Dariush Tomlinson on 19/08/2025.
//

#pragma once
#include "../../Utils/Expected.h"
#include "../../Handlers/DestinationClientHandler.h"
#include "../../Utils/DefaultConfig.h"
#include <unistd.h>
#include "../../Utils/Logger.h"
#include "../../Networking/Server.h"


/**
 * Abstract class for a receiver, which allows different implementations of receivers for different clients
 * on different threads.
 *
 * Variables:
 * destinationClientHandler - shared pointer to the destination client handler
 * stop - atomic boolean pointer which notifies every thread to shut down.
 * config - user set/default config settings which alter receiving
 *
 * Methods:
 * selectSocketTimeout - Sets a file descriptor, and waits for action
 * ClientReceiver - Constructor which initialises variables
 * receiveClients - virtual method which can be implemented by each sub class
 *
 */
class ClientReceiver {
protected:
    std::shared_ptr<DestinationClientHandler> destinationClientHandler;
    std::atomic<bool>* stop;
    DefaultConfig config;

    Expected<void> selectSocketTimeout(int socketId, int timeoutSeconds);
public:

    ClientReceiver(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop, DefaultConfig config) : destinationClientHandler(destinationClientHandler), stop(stop), config(config){};


    virtual void receiveClients(){}
};