#include "BmpWriter.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

bool BmpWriter::writeBmpFile(const std::string& filename, int width, int height, const unsigned char* data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Помилка створення файлу: " << filename << std::endl;
        return false;
    }

    size_t rowSize = ((width + 3) / 4) * 4; // Вирівнювання по 4 байти
    size_t imageDataSize = rowSize * height;
    size_t paletteSize = 256 * 4; // 4 байти на колір (BGRA)
    size_t totalFileSize = 54 + paletteSize + imageDataSize; // 54 байти заголовків + палітра + дані

    BmpHeader header;
    header.signature = 0x4D42; // 'BM'
    header.fileSize = static_cast<uint32_t>(totalFileSize);
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.dataOffset = 54 + paletteSize;

    BmpInfoHeader infoHeader;
    infoHeader.headerSize = 40;
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.planes = 1;
    infoHeader.bitsPerPixel = 8;
    infoHeader.compression = 0;
    infoHeader.imageSize = static_cast<uint32_t>(imageDataSize);
    infoHeader.xPixelsPerMeter = 2835; // 72 DPI
    infoHeader.yPixelsPerMeter = 2835; // 72 DPI
    infoHeader.colorsUsed = 256;
    infoHeader.importantColors = 256;

    writeHeader(file, header);
    writeInfoHeader(file, infoHeader);
    writeColorPalette(file);

    writeImageData(file, width, height, data);

    file.close();
    return true;
}

bool BmpWriter::writeBmpFileExact(const std::string& filename, int width, int height, 
                                 const unsigned char* data, const std::string& originalBmpPath) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Помилка створення файлу: " << filename << std::endl;
        return false;
    }

    if (!copyOriginalStructure(originalBmpPath, file, width, height, data)) {
        std::cerr << "Помилка копіювання оригінальної структури" << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}

void BmpWriter::writeHeader(std::ofstream& file, const BmpHeader& header) {
    file.write(reinterpret_cast<const char*>(&header.signature), sizeof(header.signature));
    file.write(reinterpret_cast<const char*>(&header.fileSize), sizeof(header.fileSize));
    file.write(reinterpret_cast<const char*>(&header.reserved1), sizeof(header.reserved1));
    file.write(reinterpret_cast<const char*>(&header.reserved2), sizeof(header.reserved2));
    file.write(reinterpret_cast<const char*>(&header.dataOffset), sizeof(header.dataOffset));
}

void BmpWriter::writeInfoHeader(std::ofstream& file, const BmpInfoHeader& infoHeader) {
    file.write(reinterpret_cast<const char*>(&infoHeader.headerSize), sizeof(infoHeader.headerSize));
    file.write(reinterpret_cast<const char*>(&infoHeader.width), sizeof(infoHeader.width));
    file.write(reinterpret_cast<const char*>(&infoHeader.height), sizeof(infoHeader.height));
    file.write(reinterpret_cast<const char*>(&infoHeader.planes), sizeof(infoHeader.planes));
    file.write(reinterpret_cast<const char*>(&infoHeader.bitsPerPixel), sizeof(infoHeader.bitsPerPixel));
    file.write(reinterpret_cast<const char*>(&infoHeader.compression), sizeof(infoHeader.compression));
    file.write(reinterpret_cast<const char*>(&infoHeader.imageSize), sizeof(infoHeader.imageSize));
    file.write(reinterpret_cast<const char*>(&infoHeader.xPixelsPerMeter), sizeof(infoHeader.xPixelsPerMeter));
    file.write(reinterpret_cast<const char*>(&infoHeader.yPixelsPerMeter), sizeof(infoHeader.yPixelsPerMeter));
    file.write(reinterpret_cast<const char*>(&infoHeader.colorsUsed), sizeof(infoHeader.colorsUsed));
    file.write(reinterpret_cast<const char*>(&infoHeader.importantColors), sizeof(infoHeader.importantColors));
}

void BmpWriter::writeColorPalette(std::ofstream& file) {
    // Створюємо grayscale палітру (256 кольорів)
    for (int i = 0; i < 256; ++i) {
        uint8_t color[4] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i), static_cast<uint8_t>(i), 0};
        file.write(reinterpret_cast<const char*>(color), 4);
    }
}

void BmpWriter::writeImageData(std::ofstream& file, int width, int height, const unsigned char* data) {
    // Розраховуємо розмір рядка з урахуванням вирівнювання
    size_t rowSize = ((width + 3) / 4) * 4;
    
    // у зворотному порядку
    for (int y = height - 1; y >= 0; --y) {
        // Записуємо пікселі рядка
        for (int x = 0; x < width; ++x) {
            uint8_t pixel = data[y * width + x];
            file.write(reinterpret_cast<const char*>(&pixel), 1);
        }
        
        // Додаємо зайві байти вирівнювання
        size_t paddingSize = rowSize - width;
        if (paddingSize > 0) {
            std::vector<uint8_t> padding(paddingSize, 0);
            file.write(reinterpret_cast<const char*>(padding.data()), paddingSize);
        }
    }
}

void BmpWriter::flipImageVertically(int width, int height, unsigned char* data) {
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width; ++x) {
            std::swap(data[y * width + x], 
                     data[(height - 1 - y) * width + x]);
        }
    }
}

bool BmpWriter::copyOriginalStructure(const std::string& originalBmpPath, 
                                    std::ofstream& outputFile, 
                                    int width, int height, 
                                    const unsigned char* data) {
    std::ifstream originalFile(originalBmpPath, std::ios::binary);
    if (!originalFile) {
        std::cerr << "Помилка відкриття оригінального файлу: " << originalBmpPath << std::endl;
        return false;
    }

    BmpHeader originalHeader;
    BmpInfoHeader originalInfoHeader;
    
    if (!originalFile.read(reinterpret_cast<char*>(&originalHeader), sizeof(originalHeader))) {
        std::cerr << "Помилка читання оригінального заголовка" << std::endl;
        return false;
    }
    
    if (!originalFile.read(reinterpret_cast<char*>(&originalInfoHeader), sizeof(originalInfoHeader))) {
        std::cerr << "Помилка читання оригінального Info заголовка" << std::endl;
        return false;
    }

    if (originalInfoHeader.width != width || originalInfoHeader.height != height) {
        std::cerr << "Розміри не співпадають!" << std::endl;
        return false;
    }

    outputFile.write(reinterpret_cast<const char*>(&originalHeader), sizeof(originalHeader));
    outputFile.write(reinterpret_cast<const char*>(&originalInfoHeader), sizeof(originalInfoHeader));

    if (originalInfoHeader.headerSize > 40) {
        size_t extraBytes = originalInfoHeader.headerSize - 40;
        std::vector<uint8_t> extraData(extraBytes);
        if (originalFile.read(reinterpret_cast<char*>(extraData.data()), extraBytes)) {
            outputFile.write(reinterpret_cast<const char*>(extraData.data()), extraBytes);
        }
    }

    size_t paletteSize = 0;
    if (originalInfoHeader.bitsPerPixel == 8) {
        if (originalInfoHeader.colorsUsed == 0) {
            paletteSize = 256 * 4;
        } else {
            paletteSize = originalInfoHeader.colorsUsed * 4;
        }
        
        std::vector<uint8_t> palette(paletteSize);
        if (originalFile.read(reinterpret_cast<char*>(palette.data()), paletteSize)) {
            outputFile.write(reinterpret_cast<const char*>(palette.data()), paletteSize);
        }
    }

    size_t currentPos = 14 + originalInfoHeader.headerSize + paletteSize;
    if (originalHeader.dataOffset > currentPos) {
        size_t extraBytes = originalHeader.dataOffset - currentPos;
        std::vector<uint8_t> extraData(extraBytes);
        if (originalFile.read(reinterpret_cast<char*>(extraData.data()), extraBytes)) {
            outputFile.write(reinterpret_cast<const char*>(extraData.data()), extraBytes);
        }
    }

    writeImageData(outputFile, width, height, data);

    originalFile.seekg(originalHeader.dataOffset + originalInfoHeader.imageSize);
    std::vector<uint8_t> endData;
    uint8_t byte;
    while (originalFile.read(reinterpret_cast<char*>(&byte), 1)) {
        endData.push_back(byte);
    }
    
    if (!endData.empty()) {
        outputFile.write(reinterpret_cast<const char*>(endData.data()), endData.size());
    }

    originalFile.close();
    return true;
} 
