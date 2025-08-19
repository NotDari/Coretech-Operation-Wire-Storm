#include <iostream>

#include <cstring>
#include "CTMP.h"
#include "../../Utils/Expected.h"

/**
    * Converts the data from a byte array into the class's header struct.
    * Keeps length in Network Byte order.
    * @param data header in bytes form
*/
Expected<void> CTMP::buildHeaderFromBytes(std::vector<uint8_t>& data) {
    if (data.size() != this->HEADER_SIZE) {
        return Expected<void>("Header length is incorrect", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }
    header.magicByte = data[0];
    header.initialPadding = data[1];
    //Copys the length and padding into their respective variables
    //Keeps the length in Network byte order;
    memcpy(&header.length, &data[2], sizeof(header.length));
    memcpy(&header.finalPadding, &data[4], sizeof(header.finalPadding));
    return {};
}

/**
 * Converts the header struct into a vector containg all the bytes in correct format and order.
 * @return (vector<uint8_t>) vector containing the header bytes
 */
std::vector<uint8_t> CTMP::convertHeaderToBytes() {
        std::vector<uint8_t> data(8);
        //Setting CTMP magic byte and header padding
        data[0] = header.magicByte;
        data[1] = header.initialPadding;
        //Copying data into header fields.
        memcpy(&data[2], &header.length, sizeof(header.length));
        memcpy(&data[4], &header.finalPadding, sizeof(header.finalPadding));
        return data;
}

//Converts length from Network byte order to system byte order.
uint16_t CTMP::getLength() {
    return ntohs(header.length);
}

//Validates the message is valid. Checks the magic byte matches the expected protocol.
bool CTMP::validate() {
   return (header.magicByte == MAGIC_BYTE);
}

//Moves the data from the buffer provided into the message class
void CTMP::assignData(std::vector<uint8_t>&& data) {
    this->data = std::move(data);
}

//Gets the total size of the message(header + data)
uint32_t CTMP::getTotalSize() {
    return getLength() + this->HEADER_SIZE;
}

//Converts the message from its stored version into bytes.
std::vector<uint8_t> CTMP::getDataInBytes() {
    return this->data;
}