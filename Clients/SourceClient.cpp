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


/**
 * Retrieves a set number of bytes and writes them into the buffer provided.
 * Constantly checks the socket for new data, until either the amount needed has been reached or the socket closes or an error occurs.
 *
 * @param buffer (std::vector<uint8_t>*) - buffer which the data will be written into
 * @param bytesRetrievalCount (ssize_t) - number of bytes of data to receive
 * @return - Expected containing the number of bytes retrieved if successful, or an error if one occurred.
 */
Expected<ssize_t> SourceClient::retrieveNBytes(std::vector<uint8_t>* buffer, ssize_t bytesRetrievalCount) {
    ssize_t dataReceived = 0;
    while (dataReceived < bytesRetrievalCount) {
        ssize_t numberOfBytes = recv(getSocketId(), buffer->data() + dataReceived, bytesRetrievalCount - dataReceived, 0);
        if (numberOfBytes == 0) {
            return {"Connection Closed", LoggerLevel::WARN, ErrorCode::CONNECTION_CLOSED};
        }
        if (numberOfBytes < 0) {
            return {"RecV error", LoggerLevel::ERROR, ErrorCode::DEFAULT};

        }
        dataReceived += numberOfBytes;
    }
    return {dataReceived};
}

/**
 * Retrieves and writes the header into the CTMP structure provided.
 * Throws an error if header doesn't follow protocol or one occurs retrieving data.
 *
 * @param ctmp (CTMP&) message address to write the header into
 * @return Expected containing nothing if no error, an error if one occurs
 */
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
        return {"CTMP header did not follow Protocol", LoggerLevel::WARN, ErrorCode::DEFAULT};
    }
    return{};
}

/**
 * Retrieves and writes the data into the CTMP structure provided.
 * Throws an error if data doesn't match length or one occurs retrieving data.
 * @param ctmp (CTMP&) message address to write the data into
 * @return Expected containing nothing if no error, an error if one occurs
 */
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
    int extraData = recv(getSocketId(), bufferCheck, 1, MSG_PEEK | MSG_DONTWAIT);
    if (extraData > 0) {
        return {"Data was too long for length provided", LoggerLevel::WARN, ErrorCode::DEFAULT};
    }

    //Move the data to the ctmp, instead of copying it to be more efficient
    ctmp.assignData(std::move(dataBuffer));

    //MESSAGE is fine- Continue
    return{};

}

/**
 * Calls other methods to read and write the header into the CTMP structure.
 * Returns an error if one occurs.
 *
 * @return Expected containing the CTMP message if successful, an error if not.
 */
Expected<CTMP> SourceClient::readMessage() {
    //CTMP Wrapper class
    CTMP ctmp;

    //Get header
    Expected<void> expectedHeader = readHeader(ctmp);
    if (expectedHeader.hasError()) {
        return Expected<CTMP>(expectedHeader.getError(), expectedHeader.getLoggerLevel(), expectedHeader.getErrorCode());
    }

    //Get Data
    Expected<void> expectedData = readData(ctmp);
    if (expectedData.hasError()) {
        return Expected<CTMP>(expectedData.getError(), expectedData.getLoggerLevel(), expectedData.getErrorCode());
    }

    return Expected<CTMP>(std::move(ctmp));
}