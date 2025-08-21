#ifndef IMGCOMPRESSLIB_H
#define IMGCOMPRESSLIB_H

#include "RawImageData.h"

#include <string>
#include <functional>

using ProgressCallback = std::function<void(int)>;

class ImgCompressLib
{
public:
    ImgCompressLib();
    ~ImgCompressLib();

    int encodeToFile(const RawImageData &image, const std::string &outFilename, ProgressCallback progress = nullptr);
    int decodeFromFile(RawImageData &outImage, const std::string& inFilename, ProgressCallback progress = nullptr);

private:
    class Impl;
    Impl* impl_;
};

#endif // IMGCOMPRESSLIB_H
