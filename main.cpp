#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>


using namespace std;


/**
 * Class for the whole CTMP message including the header and data.
 */
class CTMP {
private:
    const uint8_t magicByte = 0xCC;
    const uint8_t HEADER_SIZE = 8;
    /**
     * Struct for the header of CTMP which can be modified for the structure.
     */
    struct Header {
        uint8_t magicByte;
        uint8_t initialPadding;
        uint16_t length;
        uint32_t finalPadding;
    } header;
    vector<uint8_t> data;
public:
    /**
     * Converts the data from a byte array into the class's header struct.
     * Keeps length in Network Byte order.
     * @param data header in bytes form
     */
    void buildHeaderFromBytes(vector<uint8_t>& data) {
        if (data.size() != HEADER_SIZE) {
            // ERROR
        }
        header.magicByte = data[0];
        header.initialPadding = data[1];
        //Copys the length and padding into their respective variables
        //Keeps the length in Network byte order;
        memcpy(&header.length, &data[2], sizeof(header.length));
        memcpy(&header.length, &data[4], sizeof(header.finalPadding));
    }

    /**
     * Converts the header struct into a vector containg all the bytes in correct format and order.
     * @return (vector<uint8_t>) vector containing the header bytes
     */
    vector<uint8_t> convertHeaderToBytes() {
        vector<uint8_t> data(8);
        data[0] = header.magicByte;
        data[1] = header.initialPadding;
        memcpy(&data[2], &header.length, sizeof(header.length));
        memcpy(&data[4], &header.finalPadding, sizeof(header.finalPadding));
    }
    //Converts length from Network byte order to system byte order.
    uint16_t getLength() {
        return ntohs(header.length);
    }

    void validate() {
        if (header.magicByte != magicByte) {
            //ERROR Handling
        }
    }

    void assignData(vector<uint8_t>&& data) {
        this->data = std::move(data);
    }
};

/**
 * A class which will be contained by the handler.
 * Will be utilised by a thread to deliver a message to the client.
 */
class DestinationClient {
private:
    int socketId;
    //Queue of Pointers to CTMP Messages
    queue<shared_ptr<CTMP>> queue;
    //Lock for this class
    mutex destinationClientMutex;



public:
    DestinationClient(int socketId) {
        this->socketId = socketId;
    }

    /**
     * Once it gets the lock for this class,
     * pushes the message onto the queue.
     * @param message message to be added to the message queue.
     */
    void addMessageToQueue(shared_ptr<CTMP> message) {
        lock_guard lock(destinationClientMutex);
        queue.push(message);
    }

    /**
     * 
     * @return share
     */
    shared_ptr<CTMP> accessMessageItem() {
        destinationClientMutex.lock();
        shared_ptr<CTMP> message = queue.front();
        queue.pop();
        return message;
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

//TODO Change Singleton Design pattern
class DestinationClientHandler {
private:
    // The list of all the Destination Clients. Uses a pointer
    unordered_map<int, shared_ptr<DestinationClient>> destinationList;
    // A lock to prevent concurrent access of destinationList
    mutex destinationListMutex;

    //The queue of Destination Sources which are available
    deque<int> queue;


    DestinationClientHandler() {}

    //Stops singleton copying
    DestinationClientHandler(const DestinationClientHandler&) = delete;
    DestinationClientHandler& operator=(const DestinationClientHandler&) = delete;

    //Creates singleton lock and instance
    static mutex singleTonMtx;
    static DestinationClientHandler* instance;

public:

    static DestinationClientHandler* getInstance() {
        if (instance == nullptr) {
            std::lock_guard lock(singleTonMtx);
            if (instance == nullptr) {
                instance = new DestinationClientHandler();
            }
        }
        return instance;
    }

    //Called when a new destination client is added.
    //Adds it to the hashmap
    void addNewDestination(int socketId) {
        lock_guard lock(destinationListMutex);
        shared_ptr<DestinationClient> destinationClientPtr = make_shared<DestinationClient>(socketId);
        destinationList.insert({socketId,destinationClientPtr});
    }


    //Removes a destination client from the hashmap
    void removeDestination(int socketId) {
        std::lock_guard lock(destinationListMutex);
        destinationList.erase(socketId);
    }


    //Adds a message to all of the destination clients
    //Uses a Shared pointer to stop resource copying.
    void addMessage(shared_ptr<CTMP> message) {
        lock_guard lock(destinationListMutex);
        for (auto& entry : destinationList) {
            entry.second -> addMessageToQueue(message);
        }
    }







};
mutex DestinationClientHandler::singleTonMtx;
DestinationClientHandler* DestinationClientHandler::instance = nullptr;


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
    DestinationClientHandler* handler = DestinationClientHandler::getInstance();

    while (true) {
        //Accept destination client
        int destinationClient = accept(destinationSocket, (struct sockaddr *) &destinationClientAddress, &dclenIn);
        cout << "Destination client found: " << endl;
        handler->addNewDestination(destinationClient);
    }
    

    
}


/**
 * The main loop which creates both threads.
 * @return (int) - whether or not the program run was successful
 */
int main() {
    //Create Client Threads
    thread receiveDestThread(receiveDestinationClients);
    thread receiveSourceThread(receiveSourceClients);

    //Join the two threads
    receiveDestThread.join();
    receiveSourceThread.join();

    return 0;
}
