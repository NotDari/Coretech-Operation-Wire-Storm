//
// Created by Dariush Tomlinson on 19/08/2025.
//

#include "DestinationClientReceiver.h"

/**
 * Destination Client Receiver implementation of receiveClients.
 * It loops through, checking for potential destination Clients to be added.
 * Continues until signalled stop, or an error occurs.
 */
void DestinationClientReceiver::receiveClients() {
    Logger::log("Destination Client Thread Starting", LoggerLevel::INFO);

    //Set up the server
    Server server(config.destPort, config.maxClientQueue);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        Logger::log(expectedServer.getError(), expectedServer.getLoggerLevel());
        *stop = true;
        return;
    }

    Logger::log("Destination client server created successfully", LoggerLevel::INFO);


    //Loop until signalled
    while (!(*stop)) {

        //Check if new destination client available within timeout
        auto expectedSelect = selectSocketTimeout(expectedServer.getValue(), 1);
        if (expectedSelect.hasError()) {
            if (expectedSelect.getErrorCode() == ErrorCode::NO_CONNECTION_ATTEMPT) {
                //No connection. Trying again, and checking if stop has been called
                Logger::log("No connection, trying again", LoggerLevel::DEBUG);
                continue;
            }
            Logger::log("Error with destination client connection ", LoggerLevel::ERROR);
            *stop = true;
            break;
        }

        //Create Client
        Expected<int> expectedDestinationClientId = server.initiateClient();
        if (expectedDestinationClientId.hasError()) {
            Logger::log(expectedDestinationClientId.getError(), expectedDestinationClientId.getLoggerLevel());
            *stop = true;
            break;
        }
        //Add new destination
        Logger::log("Attempting to add new destination", LoggerLevel::INFO);
        auto expectedAddDestination = destinationClientHandler->addNewDestination(expectedDestinationClientId.getValue());
        if (expectedAddDestination.hasError()) {
            Logger::log(expectedAddDestination.getError(), expectedAddDestination.getLoggerLevel());
        } else {
            Logger::log("Successfully added new destination", LoggerLevel::INFO);
        }
    }
    //Signal shutdown
    destinationClientHandler->notifyAll();
    Logger::log("Destination client thread closing", LoggerLevel::INFO);
    server.stop();
}
