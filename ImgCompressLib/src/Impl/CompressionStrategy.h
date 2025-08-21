#ifndef COMPRESSIONSTRATEGY_H
#define COMPRESSIONSTRATEGY_H

#include <vector>
#include <cstdint>
#include <utility>

class BitReader;

class CompressionStrategy {

public:
    virtual ~CompressionStrategy() = default;

    virtual std::vector<uint8_t> encodeRow(const std::vector<uint8_t> &row) = 0;
    virtual std::vector<uint8_t> decodeRow(BitReader &reader, size_t rowLength) = 0;
    virtual std::pair<std::vector<uint8_t>, size_t> decodeRowWithBitCount(BitReader &reader, size_t rowLength) = 0;
};

#endif // COMPRESSIONSTRATEGY_H
