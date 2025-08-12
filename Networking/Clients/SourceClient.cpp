//
// Created by Dariush Tomlinson on 12/08/2025.
//

#include "SourceClient.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>



SourceClient::~SourceClient() = default;


Expected<ssize_t> SourceClient::retrieveNBytes(std::vector<uint8_t>* buffer, ssize_t bytesRetrievalCount) {
    ssize_t dataReceived = 0;
    while (dataReceived < bytesRetrievalCount) {
        ssize_t numberOfBytes = recv(getSocketId(), buffer->data() + dataReceived, bytesRetrievalCount - dataReceived, 0);
        if (numberOfBytes <= 0) {
            return {"Data didn't match Length", LoggerLevel::WARN};
        }
        dataReceived += numberOfBytes;
    }
    return {dataReceived};
}

Expected<int> SourceClient::readHeader(CTMP& ctmp) {
    std::vector<uint8_t> headerBuffer(headerSize);

    //Attempt to receive header of CTMP message and add it to the class
    auto expectedHeader = retrieveNBytes(&headerBuffer, headerSize);
    if (expectedHeader.hasError()) {
        return {expectedHeader.getError(), LoggerLevel::WARN};
    }
    ctmp.buildHeaderFromBytes(headerBuffer);

    //Perform header validation
    if (!ctmp.validate()) {
        return {"CTMP header did not follow Protocol", LoggerLevel::WARN};
    }
    return{0};
}

Expected<int> SourceClient::readData(CTMP& ctmp) {
    uint16_t dataLength = ctmp.getLength();

    //Create buffer for receiving data loop
    std::vector<uint8_t> dataBuffer(dataLength);


    auto expectedData = retrieveNBytes(&dataBuffer, dataLength);
    if (expectedData.hasError()) {
        return {expectedData.getError(), LoggerLevel::WARN};
    }


    //This is a check to check the length provided in the header bit is correct
    uint8_t bufferCheck[1];
    if (recv(getSocketId(), bufferCheck, 1, 0 ) <= 0) {
        return {"Data was too long for length provided", LoggerLevel::WARN};
    }

    //Move the data to the ctmp, instead of copying it to be more efficient
    ctmp.assignData(move(dataBuffer));

    //MESSAGE is fine- Continue
    return{0};

}

Expected<CTMP> SourceClient::readMessage() {
    //CTMP Wrapper class and buffer
    CTMP ctmp;
    Expected<int> expectedHeader = readHeader(ctmp);
    if (expectedHeader.hasError()) {
        return {expectedHeader.getError(), LoggerLevel::WARN};
    }
    Expected<int> expectedData = readData(ctmp);
    if (expectedData.hasError()) {
        return {expectedData.getError(), LoggerLevel::WARN};
    }

    return {std::move(ctmp)};
}