#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "CTMP.h"
#include "Handlers/DestinationClientHandler.h"
#include "Handlers/ThreadPool.h"
#include "Networking/Server.h"
#include "Networking/Clients/SourceClient.h"
#include "Utils/Logger.h"

using namespace std;



/**
 * Thread for receiving the source client.
 * Will loop through the messages sent, passing them to the Destination Client Handler.
 */
void receiveSourceClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop) {
    Logger::log("Source Client Thread Starting", LoggerLevel::INFO);
    Server server(33333, 1);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        Logger::log(expectedServer.getError(), expectedServer.getLoggerLevel());
        *stop = true;
        return;
    }
    while (!(*stop)){
        Logger::log("Searching for source client", LoggerLevel::INFO);
        //Accepting a connection to the client socket and assigning it
        Expected<int> expectedClient = server.initiateClient();
        if (expectedClient.hasError()) {
            Logger::log(expectedClient.getError(), expectedClient.getLoggerLevel());
            *stop = true;
            continue;
        }

        SourceClient sourceClient(expectedClient.getValue(), 8);
        Logger::log("Connect to Source Client", LoggerLevel::INFO);

        //Looping for messages from source client
        while (!(*stop)) {
            Expected<CTMP> expectedCTMP = sourceClient.readMessage();
            Logger::log("Started reading message", LoggerLevel::INFO);
            if (expectedCTMP.hasError()) {
                LoggerLevel level = expectedCTMP.getLoggerLevel();
                Logger::log(expectedCTMP.getError(), expectedCTMP.getLoggerLevel());
                if (level == LoggerLevel::ERROR) {
                    *stop = true;
                    break;
                }
                if (expectedCTMP.getErrorCode() == ErrorCode::ConnectionClosed) {
                    break;
                }
                continue;


            }
            //MESSAGE is fine- Continue

            Logger::log("Sending message2", LoggerLevel::INFO);
            auto expectedSendMessageAttempt = destinationClientHandler->addMessage(std::make_shared<CTMP>(std::move(expectedCTMP.getValue())));
            if (expectedSendMessageAttempt.hasError()) {
                Logger::log(expectedSendMessageAttempt.getError(), expectedSendMessageAttempt.getLoggerLevel());
            }
            break;

        }
        Logger::log("Source Client Thread Closing", LoggerLevel::INFO);
        sourceClient.closeClient();

    }


    server.stop();



}


/**
 * Thread for receiving the destination clients.
 * At the moment only gets 1 connection, but in future will loop through connections
 * and then send it to the DestinationClientHandler.
 */
void receiveDestinationClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop) {
    Logger::log("Destination Client Thread Starting", LoggerLevel::INFO);
    Server server(44444, 5);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        Logger::log(expectedServer.getError(), expectedServer.getLoggerLevel());
        *stop = true;
        return;
    }
    Logger::log("Destination client server created successfully", LoggerLevel::INFO);

    while (!(*stop)) {
        Expected<int> destinationClient = server.initiateClient();
        if (destinationClient.hasError()) {
            Logger::log(destinationClient.getError(), destinationClient.getLoggerLevel());
            *stop = true;
            break;
        }
        Logger::log("Attempting to add new destination", LoggerLevel::INFO);
        auto expectedAddDestination = destinationClientHandler->addNewDestination(destinationClient.getValue());
        if (expectedAddDestination.hasError()) {
            Logger::log(expectedAddDestination.getError(), expectedAddDestination.getLoggerLevel());
        } else {
            Logger::log("Successfully added new destination", LoggerLevel::INFO);
        }
    }
    Logger::log("Destination client thread closing", LoggerLevel::INFO);
    server.stop();

}


/**
 * The main loop which creates both threads.
 * @return (int) - whether or not the program run was successful
 */
int main() {
    Logger::log("Starting application", LoggerLevel::INFO);
    std::atomic<bool> stop{false};

    auto destinationClientHandler = std::make_shared<DestinationClientHandler>();

    //Create Client Threads
    thread receiveDestThread(receiveDestinationClients, destinationClientHandler, &stop);
    thread receiveSourceThread(receiveSourceClients, destinationClientHandler, &stop);


    ThreadPool threadPool(4, destinationClientHandler);

    //Join the two threads
    receiveDestThread.join();
    receiveSourceThread.join();

    return 0;
}
