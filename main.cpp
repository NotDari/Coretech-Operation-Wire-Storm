


#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>


using namespace  std;



void receieveSourceClients() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(33333);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    ::bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    listen(serverSocket, 5);
    cout << "Listening for connection: " << endl;

    sockaddr_in sourceClientAddress;
    socklen_t sourceAddressLength = sizeof(sourceClientAddress);

    vector<uint8_t> buffer(8);
    const uint8_t magicByte = 0xCC;

    while (true) {
        int clientSocket = accept(serverSocket, (struct sockaddr *) &sourceClientAddress, &sourceAddressLength);
        ssize_t headerBytes = recv(clientSocket, buffer.data(), buffer.size(), 0 );
        if (headerBytes < 8) {
            //ERROR HANDLING;
        }
        if (buffer[0] != magicByte) {
            //No magic byte. THrow error
        }
        //TODO Check if specific build version is little endian
        int length = (buffer[3] << 8) +  buffer[2];

        cout << "Length received:" << length << endl;
        buffer.resize(8 + length);
        ssize_t newBytes = recv(clientSocket, buffer.data() + 8, length, 0 );
        if (newBytes < 0) {
            //ERROR. Handle appropriately
        }
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

void receiveDestinationClients() {
    int destinationSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (destinationSocket < 0) {
        cout << "Destination socket failed: " << endl;
    }
    sockaddr_in destinationAddress;
    destinationAddress.sin_family = AF_INET;
    destinationAddress.sin_port = htons(44444);
    destinationAddress.sin_addr.s_addr = INADDR_ANY;
    int bindInt = ::bind(destinationSocket, (struct sockaddr *) &destinationAddress, sizeof(destinationAddress));
    if (bindInt < 0) {
        cout << "Bind failed: " << endl;
    }
    int listenInt = listen(destinationSocket, 5);
    if (listenInt < 0) {
        cout << "listen failed: " << endl;
    }
    cout << "Listening for destination connection: " << endl;

    sockaddr_in destinationClientAddress;
    socklen_t dclenIn = sizeof(destinationClientAddress);

    int destinationClient = accept(destinationSocket, (struct sockaddr *) &destinationClientAddress, &dclenIn);
    cout << "Destination client found: " << endl;

    //send(destinationClient, buffer.data(), buffer.size(), 0);
}


int main() {
    thread receiveDestThread(receiveDestinationClients);
    thread receiveSourceThread(receieveSourceClients);

    receiveDestThread.join();
    receiveSourceThread.join();

    return 0;
}
