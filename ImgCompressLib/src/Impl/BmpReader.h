#ifndef BMPREADER_H
#define BMPREADER_H

#include "../../include/RawImageData.h"
#include <string>
#include <vector>
#include <cstdint>

class BmpReader {
public:
    BmpReader() = default;
    ~BmpReader() = default;

    static RawImageData readBmpFile(const std::string& filename);
    static bool isValidBmp(const std::string& filename);

private:
    struct BmpHeader {
        uint16_t signature;      // 'BM'
        uint32_t fileSize;       // Розмір файлу
        uint16_t reserved1;      // Зарезервовано
        uint16_t reserved2;      // Зарезервовано
        uint32_t dataOffset;     // Зміщення до даних зображення
    };

    struct BmpInfoHeader {
        uint32_t headerSize;     // Розмір інформаційного заголовка
        int32_t width;           // Ширина зображення
        int32_t height;          // Висота зображення
        uint16_t planes;         // Кількість площин (завжди 1)
        uint16_t bitsPerPixel;   // Бітів на піксель
        uint32_t compression;    // Тип стискання
        uint32_t imageSize;      // Розмір зображення
        int32_t xPixelsPerMeter; // Пікселів на метр по X
        int32_t yPixelsPerMeter; // Пікселів на метр по Y
        uint32_t colorsUsed;     // Кількість кольорів
        uint32_t importantColors; // Важливі кольори
    };

    static void readHeader(std::ifstream& file, BmpHeader& header);
    static void readInfoHeader(std::ifstream& file, BmpInfoHeader& infoHeader);
    static void readImageData(std::ifstream& file, RawImageData& image, const BmpInfoHeader& infoHeader);
    static void flipImageVertically(RawImageData& image);
};

#endif // BMPREADER_H
