//
// Created by Dariush Tomlinson on 12/08/2025.
//

#include "SourceClient.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <utility>



SourceClient::~SourceClient() = default;


Expected<ssize_t> SourceClient::retrieveNBytes(std::vector<uint8_t>* buffer, ssize_t bytesRetrievalCount) {
    ssize_t dataReceived = 0;
    while (dataReceived < bytesRetrievalCount) {
        ssize_t numberOfBytes = recv(getSocketId(), buffer->data() + dataReceived, bytesRetrievalCount - dataReceived, 0);
        if (numberOfBytes == 0) {
            return {"Connection Closed", LoggerLevel::WARN, ErrorCode::ConnectionClosed};
        }
        if (numberOfBytes < 0) {
            return {"RecV error", LoggerLevel::ERROR, ErrorCode::Default};

        }
        dataReceived += numberOfBytes;
    }
    return {dataReceived};
}

Expected<void> SourceClient::readHeader(CTMP& ctmp) {
    std::vector<uint8_t> headerBuffer(headerSize);

    //Attempt to receive header of CTMP message and add it to the class
    auto expectedHeader = retrieveNBytes(&headerBuffer, headerSize);
    if (expectedHeader.hasError()) {
        return {expectedHeader.getError(), expectedHeader.getLoggerLevel(), expectedHeader.getErrorCode()};
    }
    ctmp.buildHeaderFromBytes(headerBuffer);

    //Perform header validation
    if (!ctmp.validate()) {
        return {"CTMP header did not follow Protocol", LoggerLevel::WARN, ErrorCode::Default};
    }
    return{};
}

Expected<void> SourceClient::readData(CTMP& ctmp) {
    uint16_t dataLength = ctmp.getLength();

    //Create buffer for receiving data loop
    std::vector<uint8_t> dataBuffer(dataLength);


    auto expectedData = retrieveNBytes(&dataBuffer, dataLength);
    if (expectedData.hasError()) {
        return {expectedData.getError(), expectedData.getLoggerLevel(), expectedData.getErrorCode()};
    }


    //This is a check to check the length provided in the header bit is correct
    uint8_t bufferCheck[1];
    if (recv(getSocketId(), bufferCheck, 1, 0 ) < 0) {
        return {"Data was too long for length provided", LoggerLevel::WARN, ErrorCode::Default};
    }

    //Move the data to the ctmp, instead of copying it to be more efficient
    ctmp.assignData(move(dataBuffer));

    //MESSAGE is fine- Continue
    return{};

}

Expected<CTMP> SourceClient::readMessage() {
    //CTMP Wrapper class and buffer
    CTMP ctmp;
    Expected<void> expectedHeader = readHeader(ctmp);
    if (expectedHeader.hasError()) {
        return {expectedHeader.getError(), expectedHeader.getLoggerLevel(), expectedHeader.getErrorCode()};
    }
    Expected<void> expectedData = readData(ctmp);
    if (expectedData.hasError()) {
        return {expectedData.getError(), expectedData.getLoggerLevel(), expectedData.getErrorCode()};
    }

    return {std::move(ctmp)};
}