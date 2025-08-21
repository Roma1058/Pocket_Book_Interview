#include "BitWriter.h"
#include <stdexcept>

void BitWriter::writeBit(bool bit) {
    currByte_ <<= 1;
    if (bit) currByte_ |= 1;

    ++bitsFill_;
    if (bitsFill_ == 8) {
        buff_.push_back(currByte_);
        currByte_ = 0;
        bitsFill_ = 0;
    }
}

void BitWriter::writeBits(uint32_t bits, size_t count) {
    if (count > 32)
        throw std::invalid_argument("BitWriter: count > 32");

    for (int i = static_cast<int>(count) - 1; i >= 0; --i) {
        bool bit = (bits >> i) & 1;
        writeBit(bit);
    }
}

std::vector<uint8_t> BitWriter::getBytes() const {
    std::vector<uint8_t> out = buff_;

    if (bitsFill_ > 0) {
        uint8_t lastByte = currByte_ << (8 - bitsFill_);
        out.push_back(lastByte);
    }

    return out;
}
