#include "CTMP.h"

/**
    * Converts the data from a byte array into the class's header struct.
    * Keeps length in Network Byte order.
    * @param data header in bytes form
*/
void CTMP::buildHeaderFromBytes(std::vector<uint8_t>& data) {
    if (data.size() != this->HEADER_SIZE) {
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
std::vector<uint8_t> CTMP::convertHeaderToBytes() {
        std::vector<uint8_t> data(8);
        data[0] = header.magicByte;
        data[1] = header.initialPadding;
        memcpy(&data[2], &header.length, sizeof(header.length));
        memcpy(&data[4], &header.finalPadding, sizeof(header.finalPadding));
    }
//Converts length from Network byte order to system byte order.
//TODO ask if it is part of standard library
uint16_t CTMP::getLength() {
        return ntohs(header.length);
    }

void CTMP::validate() {
        if (header.magicByte != magicByte) {
            //ERROR Handling
        }
    }

void CTMP::assignData(std::vector<uint8_t>&& data) {
        this->data = std::move(data);
    }