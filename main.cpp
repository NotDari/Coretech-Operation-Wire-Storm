#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>


using namespace std;

/**
 * A class which will be contained by the handler.
 * Will be utilised by a thread to deliver a message to the client.
 */
class DestinationClient {
private:
    int socketId;

    //TODO ADD QUEUE OF CMTP MESSAGES


public:
    DestinationClient(int socketId) {
        this->socketId = socketId;
    }




    //MAY NOT BE NECESSARY AS CONSIDERING USING HASHMAP
    //Overriding the equals so when we want to check if two clients are equal, we can.
    //This will allow us to remove it.
    bool operator==(const DestinationClient& other) const {
        return (socketId == other.socketId);
    }




};

/**
 * This class handles the list of destination clients and passing through messages.
 * It also handles the next message to send to a thread.
 */
class DestinationClientHandler {
private:
    // The list of all the Destination Clients
    unordered_map<int, DestinationClient> destinationList;
    // A lock to prevent concurrent access of destinationList
    mutex destinationListMutex;

    //The queue of Destination Sources which are available
    std::deque<int> queue;

public:

    //Called when a new destination client is added.
    //Adds it to the hashmap
    void addNewDestination(int socketId) {
        lock_guard lock(destinationListMutex);
        DestinationClient destinationClient(socketId);
        destinationList.insert({socketId,destinationClient});
    }


    //Removes a destination client from the hashmap
    void removeDestination(int socketId) {
        std::lock_guard lock(destinationListMutex);
        destinationList.erase(socketId);
    }


    //Adds a message to all of the destination clients
    void addMessage() {
        lock_guard lock(destinationListMutex);
        for (auto& entry : destinationList) {
            //TODO fill out message once destination client holds messages
        }
    }







};


/**
 * Thread for receiving the source client.
 * Will loop through the messages sent, passing them to the Destination Client Handler.
 */
void receiveSourceClients() {

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


    //Buffer and magic byte

    vector<uint8_t> buffer(8);
    const uint8_t magicByte = 0xCC;

    //Looping for messages from source client
    //Needs a lot of work
    while (true) {

        //Attempt to receive header of CMTP message
        ssize_t headerBytes = recv(clientSocket, buffer.data(), buffer.size(), 0 );

        //Check to see if error getting headerBytes
        if (headerBytes < 8) {
            //ERROR HANDLING;
        }

        //Check magic byte
        if (buffer[0] != magicByte) {
            //No magic byte. THrow error
        }

        //Convert and output length
        int length = (buffer[3] << 8) +  buffer[2];
        cout << "Length received:" << length << endl;

        //Set the buffer to the size of the entire message
        buffer.resize(8 + length);
        //TODO make a loop for receiving the bytes
        ssize_t newBytes = recv(clientSocket, buffer.data() + 8, length, 0 );


        //Check bytes have been received
        if (newBytes < 0) {
            //ERROR. Handle appropriately
        }


        //This is a check to check the length provided in the header bit is correct
        uint8_t bufferCheck[1];
        if (recv(clientSocket, bufferCheck, 1, 0 ) != 0) {
            //ERROR. DROP
        }

        //MESSAGE is fine- Continue

        close(clientSocket);
        break;

    }



    close(serverSocket);
}


/**
 * Thread for receiving the destination clients.
 * At the moment only gets 1 connection, but in future will loop through connections
 * and then send it to the DestinationClientHandler.
 */
void receiveDestinationClients() {
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
    
    //Accept destination client
    int destinationClient = accept(destinationSocket, (struct sockaddr *) &destinationClientAddress, &dclenIn);
    cout << "Destination client found: " << endl;
    
}


/**
 * The main loop which creates both threads.
 * @return (int) - whether or not the program run was successful
 */
int main() {
    //Create Destination Client Thread
    thread receiveDestThread(receiveDestinationClients);
    //Create source client thread
    thread receiveSourceThread(receiveSourceClients);

    //Join the two threads
    receiveDestThread.join();
    receiveSourceThread.join();

    return 0;
}
