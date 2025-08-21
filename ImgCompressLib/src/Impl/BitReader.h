#include <vector>
#include <cstdint>

class BitReader {
public:
    explicit BitReader(const std::vector<uint8_t>& data);

    bool readBit();
    uint32_t readBits(size_t count);
    bool hasMore() const;
    void setPosition(size_t byteIndex, int bitIndex = 0);

private:
    const std::vector<uint8_t>& data_;
    size_t byteIndex_ = 0;
    int bitIndex_ = 0;
};
