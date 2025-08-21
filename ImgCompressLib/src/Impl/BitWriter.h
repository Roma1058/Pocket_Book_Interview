#include <vector>
#include <cstdint>

class BitWriter {
public:
    void writeBit(bool bit);
    void writeBits(uint32_t bits, size_t count);
    std::vector<uint8_t> getBytes() const;

private:
    std::vector<uint8_t> buff_;
    uint8_t currByte_ = 0;
    int bitsFill_ = 0;
};
