#pragma once

#include <cstdint>
#include <unistd.h>
#include <vector>
/**
    * Class for the whole CTMP message including the header and data.
 */
class CTMP {
private:
    static constexpr  uint8_t magicByte = 0xCC;
    static constexpr  uint8_t HEADER_SIZE = 8;
    struct Header {
        uint8_t magicByte;
        uint8_t initialPadding;
        uint16_t length;
        uint32_t finalPadding;
    } header;
    std::vector<uint8_t> data;
public:
    void buildHeaderFromBytes(std::vector<uint8_t>& data);

    std::vector<uint8_t> convertHeaderToBytes();


    uint16_t getLength();

    uint32_t getTotalSize();

    bool validate();

    void assignData(std::vector<uint8_t>&& data);

    std::vector<uint8_t>  getDataInBytes();

};