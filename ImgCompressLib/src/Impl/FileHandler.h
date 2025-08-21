#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <functional>
#include <vector>
#include <cstdint>

#include "../../include/RawImageData.h"
#include "CompressionStrategy.h"

using ProgressCallback = std::function<void(int)>;

class FileHandler
{
public:
    FileHandler();
    ~FileHandler();

    void writeToFile(const RawImageData& image,
                     const std::string& filename,
                     ProgressCallback progress = nullptr);

    RawImageData readFromFile(const std::string& filename,
                              ProgressCallback progress = nullptr);

private:
    void encodeRow(const unsigned char* row, size_t width, std::vector<uint8_t>& out);
    void decodeRow(const std::vector<uint8_t>& in, size_t& offset,
                   unsigned char* row, size_t width);

    uint32_t computeCRC32(const std::vector<uint8_t>& data);

    CompressionStrategy* strategy_;
};

#endif // FILEHANDLER_H 
