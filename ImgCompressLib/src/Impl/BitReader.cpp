#include "BitReader.h"
#include <stdexcept>

BitReader::BitReader(const std::vector<uint8_t>& data)
    : data_(data), byteIndex_(0), bitIndex_(0) {}

bool BitReader::readBit() {
    if (byteIndex_ >= data_.size())
        throw std::out_of_range("BitReader: no more data");

    bool bit = (data_[byteIndex_] >> (7 - bitIndex_)) & 1;

    ++bitIndex_;
    if (bitIndex_ == 8) {
        bitIndex_ = 0;
        ++byteIndex_;
    }

    return bit;
}

uint32_t BitReader::readBits(size_t count) {
    if (count > 32)
        throw std::invalid_argument("BitReader: count > 32");

    uint32_t value = 0;
    for (size_t i = 0; i < count; ++i) {
        value = (value << 1) | static_cast<uint32_t>(readBit());
    }
    return value;
}

bool BitReader::hasMore() const {
    return byteIndex_ < data_.size() || (byteIndex_ == data_.size() && bitIndex_ != 0);
}

void BitReader::setPosition(size_t byteIndex, int bitIndex) {
    if (byteIndex > data_.size() || bitIndex < 0 || bitIndex > 7) {
        throw std::out_of_range("BitReader: invalid position");
    }
    byteIndex_ = byteIndex;
    bitIndex_ = bitIndex;
}
