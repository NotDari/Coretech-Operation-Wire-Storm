#pragma once
#include "Client.h"
#include "../Utils/Expected.h"
#include "../Networking/Protocols/CTMP.h"


/**
 * An implementation of the base Client Class.
 * Represents a Sourrce Client and handles the retrieval of the messages(the header and the data).
 *
 * Variables:
 * headerSize - the size of the CTMP header
 *
 * Methods:
 * retrieveNBytes - retrieves a set number of bytes from the client socket
 * readHeader - retrieves the header part of the message
 * readData - retrieves the data part of the message
 * SourceClient - Constructor, creating a new base client, and sets headerSize
 * readMessage - Handler of the whole process, reading both header and data
 *
 */
class SourceClient : public Client {
private:
    int headerSize;

    Expected<ssize_t> retrieveNBytes(std::vector<uint8_t>* buffer, ssize_t  bytesRetrievalCount);

    Expected<void> readHeader(CTMP& ctmp);
    Expected<void> readData(CTMP& ctmp);

public:


    SourceClient(int socketId, int headerSize) : Client(socketId), headerSize(headerSize){};

    Expected<CTMP> readMessage();




};
