#pragma once

#include <cstdint>
#include <unistd.h>
#include <vector>
/**
    * Class for the whole CTMP message including the header and data.
 */
class CTMP {
private:
    //Magic byte which needs to be validated
    static constexpr uint8_t magicByte = 0xCC;
    static constexpr uint8_t HEADER_SIZE = 8;
    //Representation of the CTMP header fields
    struct Header {
        uint8_t magicByte;
        uint8_t initialPadding;
        uint16_t length;
        uint32_t finalPadding;
    } header;
    //Rest of the payload with length defined in header
    std::vector<uint8_t> data;
public:
    //Converts header from bytes and populates the header struct instance
    void buildHeaderFromBytes(std::vector<uint8_t>& data);

    //Converts this class' header instance back into bytes
    std::vector<uint8_t> convertHeaderToBytes();

    //Get the length described in the header
    uint16_t getLength();

    //Get the total size of all bytes needed for a message
    uint32_t getTotalSize();

    //Performs necessary validation checks on the header
    bool validate();

    //Assigns the main data of this CTMP message
    void assignData(std::vector<uint8_t>&& data);

    //Getter for data
    std::vector<uint8_t> getDataInBytes();

};