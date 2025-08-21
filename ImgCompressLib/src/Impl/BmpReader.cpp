#include "BmpReader.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstring>

RawImageData BmpReader::readBmpFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Не вдалося відкрити файл: " + filename);
    }

    BmpHeader header;
    BmpInfoHeader infoHeader;
    
    readHeader(file, header);
    readInfoHeader(file, infoHeader);

    if (header.signature != 0x4D42) { // 'BM'
        throw std::runtime_error("Невірний формат BMP файлу");
    }

    if (infoHeader.bitsPerPixel != 8) {
        throw std::runtime_error("Підтримуються тільки 8-бітні grayscale зображення");
    }

    if (infoHeader.compression != 0) {
        throw std::runtime_error("Підтримуються тільки нестиснуті BMP файли");
    }

    RawImageData image;
    image.width = infoHeader.width;
    image.height = infoHeader.height;
    image.data = new unsigned char[image.width * image.height];

    readImageData(file, image, infoHeader);

    file.close();
    return image;
}

bool BmpReader::isValidBmp(const std::string& filename) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        uint16_t signature;
        file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
        file.close();

        return signature == 0x4D42; // 'BM'
    } catch (...) {
        return false;
    }
}

void BmpReader::readHeader(std::ifstream& file, BmpHeader& header) {
    file.read(reinterpret_cast<char*>(&header.signature), sizeof(header.signature));
    file.read(reinterpret_cast<char*>(&header.fileSize), sizeof(header.fileSize));
    file.read(reinterpret_cast<char*>(&header.reserved1), sizeof(header.reserved1));
    file.read(reinterpret_cast<char*>(&header.reserved2), sizeof(header.reserved2));
    file.read(reinterpret_cast<char*>(&header.dataOffset), sizeof(header.dataOffset));
}

void BmpReader::readInfoHeader(std::ifstream& file, BmpInfoHeader& infoHeader) {
    file.read(reinterpret_cast<char*>(&infoHeader.headerSize), sizeof(infoHeader.headerSize));
    file.read(reinterpret_cast<char*>(&infoHeader.width), sizeof(infoHeader.width));
    file.read(reinterpret_cast<char*>(&infoHeader.height), sizeof(infoHeader.height));
    file.read(reinterpret_cast<char*>(&infoHeader.planes), sizeof(infoHeader.planes));
    file.read(reinterpret_cast<char*>(&infoHeader.bitsPerPixel), sizeof(infoHeader.bitsPerPixel));
    file.read(reinterpret_cast<char*>(&infoHeader.compression), sizeof(infoHeader.compression));
    file.read(reinterpret_cast<char*>(&infoHeader.imageSize), sizeof(infoHeader.imageSize));
    file.read(reinterpret_cast<char*>(&infoHeader.xPixelsPerMeter), sizeof(infoHeader.xPixelsPerMeter));
    file.read(reinterpret_cast<char*>(&infoHeader.yPixelsPerMeter), sizeof(infoHeader.yPixelsPerMeter));
    file.read(reinterpret_cast<char*>(&infoHeader.colorsUsed), sizeof(infoHeader.colorsUsed));
    file.read(reinterpret_cast<char*>(&infoHeader.importantColors), sizeof(infoHeader.importantColors));
}

void BmpReader::readImageData(std::ifstream& file, RawImageData& image, const BmpInfoHeader& infoHeader) {
    uint32_t colorsUsed = infoHeader.colorsUsed;
    if (colorsUsed == 0) {
        colorsUsed = 256;
    }
    
    size_t paletteSize = colorsUsed * 4; // 4 байти на колір (BGRA)
    file.seekg(54 + paletteSize); // 54 байти заголовків + палітра

    size_t rowSize = ((image.width + 3) / 4) * 4; // Вирівнювання по 4 байти

    for (int y = image.height - 1; y >= 0; --y) {
        for (int x = 0; x < image.width; ++x) {
            unsigned char pixel;
            file.read(reinterpret_cast<char*>(&pixel), 1);
            image.data[y * image.width + x] = pixel;
        }
        
        if (rowSize > static_cast<size_t>(image.width)) {
            file.seekg(rowSize - image.width, std::ios::cur);
        }
    }
}

void BmpReader::flipImageVertically(RawImageData& image) {
    for (int y = 0; y < image.height / 2; ++y) {
        for (int x = 0; x < image.width; ++x) {
            std::swap(image.data[y * image.width + x], 
                     image.data[(image.height - 1 - y) * image.width + x]);
        }
    }
} 
