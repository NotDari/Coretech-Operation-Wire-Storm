#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "CTMP.h"
#include "Clients/DestinationClientHandler.h"

using namespace std;


/**
 * Thread for receiving the source client.
 * Will loop through the messages sent, passing them to the Destination Client Handler.
 */
void receiveSourceClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler) {

    //Creating the server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);


    //Creating the details for the binding of the server socket
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(33333);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    ::bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));


    //Listening/Waiting for a connection to the server socket
    listen(serverSocket, 1);
    cout << "Listening for connection: " << endl;


    //Accepting a connection to the client socket and assigning it
    sockaddr_in sourceClientAddress;
    socklen_t sourceAddressLength = sizeof(sourceClientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &sourceClientAddress, &sourceAddressLength);







    //Looping for messages from source client
    //Needs a lot of work
    while (true) {
        //CTMP Wrapper class and buffer
        CTMP ctmp;
        vector<uint8_t> headerBuffer(8);

        //Attempt to receive header of CMTP message and add it to the class
        ssize_t headerBytes = recv(clientSocket, headerBuffer.data(), headerBuffer.size(), 0 );
        ctmp.buildHeaderFromBytes(headerBuffer);

        //Perform header validation
        ctmp.validate();

        uint16_t dataLength = ctmp.getLength();

        //Create buffer for receiving data loop
        vector<uint8_t> dataBuffer(dataLength);
        size_t dataReceived = 0;

        //Loop through, getting all the message
        while (dataReceived < dataLength) {
            ssize_t numberOfBytes = recv(clientSocket, dataBuffer.data() + dataReceived, dataLength - dataReceived, 0);
            if (numberOfBytes <= 0) {
                //ERROR due to inconmplete data
            }
            dataReceived += numberOfBytes;
        }


        //This is a check to check the length provided in the header bit is correct
        uint8_t bufferCheck[1];
        if (recv(clientSocket, bufferCheck, 1, 0 ) != 0) {
            //ERROR. DROP
        }

        //Move the data to the ctmp, instead of copying it to be more efficient
        ctmp.assignData(move(dataBuffer));

        //MESSAGE is fine- Continue

    }

    close(clientSocket);



    close(serverSocket);
}


/**
 * Thread for receiving the destination clients.
 * At the moment only gets 1 connection, but in future will loop through connections
 * and then send it to the DestinationClientHandler.
 */
void receiveDestinationClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler) {
    //Create destination client and check it connected 
    int destinationSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (destinationSocket < 0) {
        cout << "Destination socket failed: " << endl;
    }

    //Create socket binder and attempt bind
    sockaddr_in destinationAddress;
    destinationAddress.sin_family = AF_INET;
    destinationAddress.sin_port = htons(44444);
    destinationAddress.sin_addr.s_addr = INADDR_ANY;
    int bindInt = ::bind(destinationSocket, (struct sockaddr *) &destinationAddress, sizeof(destinationAddress));
    //Check bind successful
    if (bindInt < 0) {
        cout << "Bind failed: " << endl;
    }
    //Wait for client to attempt to connect
    int listenInt = listen(destinationSocket, 5);
    if (listenInt < 0) {
        cout << "listen failed: " << endl;
    }
    cout << "Listening for destination connection: " << endl;

    
    sockaddr_in destinationClientAddress;
    socklen_t dclenIn = sizeof(destinationClientAddress);

    while (true) {
        //Accept destination client
        int destinationClient = accept(destinationSocket, (struct sockaddr *) &destinationClientAddress, &dclenIn);
        cout << "Destination client found: " << endl;
        destinationClientHandler->addNewDestination(destinationClient);
    }
    

    
}


/**
 * The main loop which creates both threads.
 * @return (int) - whether or not the program run was successful
 */
int main() {

    auto destinationClientHandler = std::make_shared<DestinationClientHandler>();

    //Create Client Threads
    thread receiveDestThread(receiveDestinationClients, destinationClientHandler);
    thread receiveSourceThread(receiveSourceClients, destinationClientHandler);

    //Join the two threads
    receiveDestThread.join();
    receiveSourceThread.join();

    return 0;
}
