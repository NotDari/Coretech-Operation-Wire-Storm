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

using namespace std;



/**
 * Thread for receiving the source client.
 * Will loop through the messages sent, passing them to the Destination Client Handler.
 */
void receiveSourceClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop) {
    Server server(33333);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        //TODO(Log)
        *stop = true;
        return;
    }

    //Accepting a connection to the client socket and assigning it
    Expected<int> expectedClient = server.initiateClient();
    if (expectedClient.hasError()) {
        //TODO(Log)
        *stop = true;
        return;
    }

    SourceClient sourceClient(expectedClient.getValue(), 8);

    //Looping for messages from source client
    while (!(*stop)) {
        Expected<CTMP> expectedCTMP = sourceClient.readMessage();
        if (expectedCTMP.hasError()) {
            //TODO(Log)
            *stop = true;
            break;
        }
        //MESSAGE is fine- Continue
        destinationClientHandler->addMessage(std::make_shared<CTMP>(std::move(expectedCTMP.getValue())));

    }

    sourceClient.closeClient();
    server.stop();



}


/**
 * Thread for receiving the destination clients.
 * At the moment only gets 1 connection, but in future will loop through connections
 * and then send it to the DestinationClientHandler.
 */
void receiveDestinationClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop) {

    Server server(44444);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        //TODO(Log)
        *stop = true;
        return;
    }



    while (!(*stop)) {
        Expected<int> destinationClient = server.initiateClient();
        if (destinationClient.hasError()) {
            //TODO(Log)
            *stop = true;
            break;
        }
        destinationClientHandler->addNewDestination(destinationClient.getValue());
    }
    server.stop();
}


/**
 * The main loop which creates both threads.
 * @return (int) - whether or not the program run was successful
 */
int main() {
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
