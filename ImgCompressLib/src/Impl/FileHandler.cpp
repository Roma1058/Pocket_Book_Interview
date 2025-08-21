#include "FileHandler.h"
#include "Block4Strategy.h"
#include "BitReader.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cstring>

static const char FILE_HEADER[2] = {'B', 'A'};

FileHandler::FileHandler() {
    strategy_ = new Block4Strategy();
}

FileHandler::~FileHandler() {
    delete strategy_;
}

uint32_t FileHandler::computeCRC32(const std::vector<uint8_t>& data)
{
    uint32_t crc = 0xFFFFFFFF;
    for (auto b : data) {
        crc ^= b;
        for (int i = 0; i < 8; ++i)
            crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : (crc >> 1);
    }
    return ~crc;
}

void FileHandler::encodeRow(const unsigned char* row, size_t width, std::vector<uint8_t>& out)
{
    std::vector<uint8_t> tmp(row, row + width);
    std::vector<uint8_t> encoded = strategy_->encodeRow(tmp);
    out.insert(out.end(), encoded.begin(), encoded.end());
}

void FileHandler::decodeRow(const std::vector<uint8_t>& in, size_t& offset,
                            unsigned char* row, size_t width)
{
    std::vector<uint8_t> remainingData(in.begin() + offset, in.end());
    BitReader reader(remainingData);
    
    auto result = strategy_->decodeRowWithBitCount(reader, width);
    std::vector<uint8_t> decoded = result.first;
    size_t bitsRead = result.second;
    
    std::copy(decoded.begin(), decoded.end(), row);
    
    offset += (bitsRead + 7) / 8;
}

void FileHandler::writeToFile(const RawImageData& image,
                              const std::string& filename,
                              ProgressCallback progress)
{
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Cannot open file for writing");

    out.write(FILE_HEADER, 2);
    out.write(reinterpret_cast<const char*>(&image.width), sizeof(int));
    out.write(reinterpret_cast<const char*>(&image.height), sizeof(int));

    std::vector<uint8_t> rowIndex((image.height + 7)/8, 0);
    for (int y = 0; y < image.height; ++y) {
        bool empty = true;
        for (int x = 0; x < image.width; ++x) {
            if (image.data[y*image.width + x] != 0xFF) {
                empty = false;
                break;
            }
        }
        if (!empty) rowIndex[y/8] |= (1 << (y%8));
    }
    out.write(reinterpret_cast<const char*>(rowIndex.data()), rowIndex.size());

    std::vector<uint8_t> compressed;
    for (int y = 0; y < image.height; ++y) {
        if ((rowIndex[y/8] >> (y%8)) & 1) {
            encodeRow(image.data + y*image.width, image.width, compressed);
        }
        if (progress) progress(static_cast<int>(100.0*(y+1)/image.height));
    }

    out.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());

    // CRC32
    std::vector<uint8_t> crcBuf;
    crcBuf.insert(crcBuf.end(), FILE_HEADER, FILE_HEADER+2);
    crcBuf.insert(crcBuf.end(),
                  reinterpret_cast<const uint8_t*>(&image.width),
                  reinterpret_cast<const uint8_t*>(&image.width)+sizeof(int));
    crcBuf.insert(crcBuf.end(),
                  reinterpret_cast<const uint8_t*>(&image.height),
                  reinterpret_cast<const uint8_t*>(&image.height)+sizeof(int));
    crcBuf.insert(crcBuf.end(), rowIndex.begin(), rowIndex.end());
    crcBuf.insert(crcBuf.end(), compressed.begin(), compressed.end());

    uint32_t crc = computeCRC32(crcBuf);
    out.write(reinterpret_cast<const char*>(&crc), sizeof(crc));
    out.close();
}

RawImageData FileHandler::readFromFile(const std::string& filename,
                                       ProgressCallback progress)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open())
        throw std::runtime_error("Cannot open file for reading");

    char header[2];
    in.read(header, 2);
    if (header[0]!=FILE_HEADER[0] || header[1]!=FILE_HEADER[1])
        throw std::runtime_error("Invalid file format");

    int width, height;
    in.read(reinterpret_cast<char*>(&width), sizeof(int));
    in.read(reinterpret_cast<char*>(&height), sizeof(int));

    std::vector<uint8_t> rowIndex((height+7)/8);
    in.read(reinterpret_cast<char*>(rowIndex.data()), rowIndex.size());

    std::vector<uint8_t> compressed((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());

    if (compressed.size() < 4)
        throw std::runtime_error("File too short for CRC");

    uint32_t fileCrc;
    std::memcpy(&fileCrc, &compressed[compressed.size()-4], 4);
    compressed.resize(compressed.size()-4);

    std::vector<uint8_t> crcBuf;
    crcBuf.insert(crcBuf.end(), FILE_HEADER, FILE_HEADER+2);
    crcBuf.insert(crcBuf.end(),
                  reinterpret_cast<const uint8_t*>(&width),
                  reinterpret_cast<const uint8_t*>(&width)+sizeof(int));
    crcBuf.insert(crcBuf.end(),
                  reinterpret_cast<const uint8_t*>(&height),
                  reinterpret_cast<const uint8_t*>(&height)+sizeof(int));
    crcBuf.insert(crcBuf.end(), rowIndex.begin(), rowIndex.end());
    crcBuf.insert(crcBuf.end(), compressed.begin(), compressed.end());

    uint32_t crc = computeCRC32(crcBuf);
    if (crc != fileCrc)
        throw std::runtime_error("CRC check failed");

    RawImageData image;
    image.width = width;
    image.height = height;
    image.data = new unsigned char[width*height];

    size_t offset = 0;
    for (int y = 0; y < height; ++y) {
        if ((rowIndex[y/8] >> (y%8)) & 1) {
            decodeRow(compressed, offset, image.data + y*width, width);
        } else {
            // порожній рядок
            std::memset(image.data + y*width, 0xFF, width);
        }
        if (progress) progress(static_cast<int>(100.0*(y+1)/height));
    }

    return image;
}
