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

    sockaddr_in clientAddress;
    socklen_t clenIn = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clenIn);

    cout << "Client connected + ip:" << clientAddress.sin_addr.s_addr << endl;

    close(serverSocket);
    return 0;
}

