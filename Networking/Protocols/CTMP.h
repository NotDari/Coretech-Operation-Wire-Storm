#pragma once

#include <cstdint>
#include <unistd.h>
#include <vector>

#include "../../Utils/Expected.h"

/**
 * Class for the CTMP message including the header and the data.
 *
 * Variables:
 * MAGIC_BYTE - Magic byte which needs to be validated
 * HEADER_SIZE - Size of the CTMP header
 * header - Struct representation of CTMP header fields
 * data - Rest of the payload with length defined in header
 *
 * Methods:
 * buildHeaderFromBytes - Converts header from bytes and populates the header struct instance
 * convertHeaderToBytes - Converts this class' header instance back into bytes
 * getLength - Get the length described in the header
 * getTotalSize - Get the total size of all bytes needed for a message
 * validate - Performs necessary validation checks on the header
 * assignData - Assigns the main data of this CTMP message
 * getDataInBytes - Getter for data
 */
class CTMP {
private:
    static constexpr uint8_t MAGIC_BYTE = 0xCC;
    static constexpr uint8_t HEADER_SIZE = 8;

    //Representation of the CTMP header fields
    struct Header {
        uint8_t magicByte;
        uint8_t initialPadding;
        uint16_t length;
        uint32_t finalPadding;
    } header;

    std::vector<uint8_t> data;
public:
    Expected<void> buildHeaderFromBytes(std::vector<uint8_t>& data);

    std::vector<uint8_t> convertHeaderToBytes();

    uint16_t getLength();

    uint32_t getTotalSize();

    bool validate();

    void assignData(std::vector<uint8_t>&& data);

    std::vector<uint8_t> getDataInBytes();

};