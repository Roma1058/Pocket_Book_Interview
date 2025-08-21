#ifndef BLOCK4STRATEGY_H
#define BLOCK4STRATEGY_H

#include "CompressionStrategy.h"

class Block4Strategy : public CompressionStrategy
{
public:
    std::vector<uint8_t> encodeRow(const std::vector<uint8_t> &row) override;
    std::vector<uint8_t> decodeRow(BitReader &reader, size_t rowLength) override;
    std::pair<std::vector<uint8_t>, size_t> decodeRowWithBitCount(BitReader &reader, size_t rowLength) override;
};

#endif // BLOCK4STRATEGY_H
