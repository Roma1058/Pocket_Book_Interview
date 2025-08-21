#ifndef BMPWRITER_H
#define BMPWRITER_H

#include <string>
#include <vector>
#include <cstdint>

struct BmpHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
};

struct BmpInfoHeader {
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t importantColors;
};

class BmpWriter {
public:
    static bool writeBmpFile(const std::string& filename, int width, int height, const unsigned char* data);
    static bool writeBmpFileExact(const std::string& filename, int width, int height, 
                                 const unsigned char* data, const std::string& originalBmpPath);

private:
    static void writeHeader(std::ofstream& file, const BmpHeader& header);
    static void writeInfoHeader(std::ofstream& file, const BmpInfoHeader& infoHeader);
    static void writeColorPalette(std::ofstream& file);
    static void writeImageData(std::ofstream& file, int width, int height, const unsigned char* data);
    static void flipImageVertically(int width, int height, unsigned char* data);

    static bool copyOriginalStructure(const std::string& originalBmpPath, 
                                   std::ofstream& outputFile, 
                                   int width, int height, 
                                   const unsigned char* data);
};

#endif // BMPWRITER_H 
