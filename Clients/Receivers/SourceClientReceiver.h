//
// Created by Dariush Tomlinson on 19/08/2025.
//

#pragma once
#include "ClientReceiver.h"

/**
 * Source Client Implementation of abstract ClientReceiver.
 * Retrieves one source client and its messages.
 *
 * Methods:
 * SourceClientReceiver - Constructor, calls super constructor
 * receiveClients - implementation of virtual methods, receives the source client and its messages
 */
class SourceClientReceiver : public ClientReceiver {
public:
    SourceClientReceiver(std::shared_ptr<DestinationClientHandler> handler,std::atomic<bool>* stop, DefaultConfig config) : ClientReceiver(handler, stop, config) {}

    void receiveClients() override;
};
