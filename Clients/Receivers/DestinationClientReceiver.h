//
// Created by Dariush Tomlinson on 19/08/2025.
//

#pragma once
#include "ClientReceiver.h"

/**
 * Destination Client Implementation of abstract ClientReceiver.
 * Retrieves Destination Clients.
 *
 * Methods:
 * DestinationClientReceiver - Constructor, calls super constructor
 * receiveClients - implementation of virtual methods, receives the destination clients
 */
class DestinationClientReceiver : public ClientReceiver {
public:
    DestinationClientReceiver(std::shared_ptr<DestinationClientHandler> handler,std::atomic<bool>& stop, DefaultConfig config) : ClientReceiver(handler, stop, config) {}

    void receiveClients() override;
};
