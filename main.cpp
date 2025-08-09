#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


using namespace  std;



int main() {
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

    int clientSocket = accept(serverSocket, (struct sockaddr *) &sourceClientAddress, &sourceAddressLength);

    int bufferSize = 4096;
    uint8_t buffer[bufferSize];
    const uint8_t magicByte = 0xCC;

    while (true) {
        ssize_t headerBytes = recv(clientSocket, buffer, 8, 0 );
        if (headerBytes < 8) {
            //ERROR HANDLING;
        }
        if (buffer[0] != magicByte) {
            //No magic byte. THrow error
        }
        //TODO Check if specific build version is little endian
        int length = (buffer[3] << 8) +  buffer[2];

        cout << "Length received:" << length << endl;
        break;

    }
    close(serverSocket);
    return 0;
}



