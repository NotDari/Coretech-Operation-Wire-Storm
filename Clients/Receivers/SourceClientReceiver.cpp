//
// Created by Dariush Tomlinson on 19/08/2025.
//
#include "SourceClientReceiver.h"
#include "../SourceClient.h"

/**
 * Source Client Receiver implementation of receiveClients.
 * It loops through, checking if one source client can be added.
 * While the client is connected, it continuously recieves messages, adding them to the
 * handler's queue of messages to be sent.
 * Continues until signalled stop, or an error occurs.
 */
void SourceClientReceiver::receiveClients() {
    Logger::log("Source Client Thread Starting", LoggerLevel::INFO);
    //Create server
    Server server(config.sourcePort, 1);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        Logger::log(expectedServer.getError(), expectedServer.getLoggerLevel());
        *stop = true;
        return;
    }
    std::shared_ptr<SourceClient> sourceClient;
    //Loop until signalled to stop
    while (!(*stop)){
        Logger::log("Searching for source client connection/message", LoggerLevel::DEBUG);

        //Check if source client is connected to search for either client or messages
        auto expectedSelect = selectSocketTimeout(
            (!sourceClient) ? expectedServer.getValue() : sourceClient->getSocketId()
            ,1
        );

        //Check if error
        if (expectedSelect.hasError()) {
            if (expectedSelect.getErrorCode() == ErrorCode::NO_CONNECTION_ATTEMPT) {
                //No connection. Trying again, and checking if stop has been called
                Logger::log("No connection, trying again", LoggerLevel::DEBUG);
                continue;
            }
            Logger::log("Error with sourceClient connection ", LoggerLevel::ERROR);
            *stop = true;
            break;
        }

        //If no source client, try connect to source client.
        if (!sourceClient) {
            //Accepting a connection to the client socket and assigning it
            Expected<int> expectedClient = server.initiateClient();
            if (expectedClient.hasError()) {
                Logger::log(expectedClient.getError(), expectedClient.getLoggerLevel());
                *stop = true;
                continue;
            }

            sourceClient = std::make_shared<SourceClient>(expectedClient.getValue(), 8);
            Logger::log("Connected to Source Client", LoggerLevel::INFO);
        }
        //There is a source client so try to get the messages
        else {
            Expected<CTMP> expectedCTMP = sourceClient->readMessage();
            Logger::log("Started reading message", LoggerLevel::DEBUG);
            if (expectedCTMP.hasError()) {
                LoggerLevel level = expectedCTMP.getLoggerLevel();
                Logger::log(expectedCTMP.getError(), expectedCTMP.getLoggerLevel());
                if (level == LoggerLevel::ERROR) {
                    *stop = true;
                    break;
                }
                if (expectedCTMP.getErrorCode() == ErrorCode::CONNECTION_CLOSED) {
                    sourceClient.reset();
                    Logger::log("Client disconnected", LoggerLevel::INFO);
                }
                continue;


            }
            //MESSAGE is fine- Continue

            Logger::log("Adding message", LoggerLevel::DEBUG);
            auto expectedSendMessageAttempt = destinationClientHandler->addMessage(std::make_shared<CTMP>(std::move(expectedCTMP.getValue())));
            if (expectedSendMessageAttempt.hasError()) {
                Logger::log(expectedSendMessageAttempt.getError(), expectedSendMessageAttempt.getLoggerLevel());
            }

        }




    }
    //Shutdown everything
    Logger::log("Source Client Thread Closing", LoggerLevel::INFO);
    if (sourceClient){
        sourceClient->closeClient();
    }
    destinationClientHandler->notifyAll();
    server.stop();
}