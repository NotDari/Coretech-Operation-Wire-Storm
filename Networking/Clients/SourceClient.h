#pragma once
#include "Client.h"
#include "../../Utils/Expected.h"
#include "../../CTMP.h"


class SourceClient : public Client {
private:
    int headerSize;

    Expected<ssize_t> retrieveNBytes(std::vector<uint8_t>* buffer, ssize_t  bytesRetrievalCount);

    Expected<void> readHeader(CTMP& ctmp);
    Expected<void> readData(CTMP& ctmp);

public:

    ~SourceClient();

    SourceClient(int socketId, int headerSize) : Client(socketId), headerSize(headerSize){};

    Expected<CTMP> readMessage();


};
