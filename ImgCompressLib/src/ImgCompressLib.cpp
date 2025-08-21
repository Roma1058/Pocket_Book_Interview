#include "../include/ImgCompressLib.h"
#include "Impl/FileHandler.h"

#include <stdexcept>
#include <iostream>

class ImgCompressLib::Impl
{
public:
    Impl() = default;

    int encodeToFile(const RawImageData &image,
                     const std::string &outFilename,
                     ProgressCallback progress)
    {
        try {
            FileHandler fh;
            fh.writeToFile(image, outFilename, progress);
            return 0; // успіх
        } catch (const std::exception &ex) {
            std::cerr << "Encode failed: " << ex.what() << std::endl;
            return 1;
        }
    }

    int decodeFromFile(RawImageData &outImage,
                       const std::string &inFilename,
                       ProgressCallback progress)
    {
        try {
            FileHandler fh;
            outImage = fh.readFromFile(inFilename, progress);
            return 0;
        } catch (const std::exception &ex) {
            std::cerr << "Decode failed: " << ex.what() << std::endl;
            return 1;
        }
    }
};

ImgCompressLib::ImgCompressLib()
    : impl_(new Impl())
{
}

ImgCompressLib::~ImgCompressLib()
{
    delete impl_;
}

int ImgCompressLib::encodeToFile(const RawImageData &image,
                                 const std::string &outFilename,
                                 ProgressCallback progress)
{
    return impl_->encodeToFile(image, outFilename, progress);
}

int ImgCompressLib::decodeFromFile(RawImageData &outImage,
                                   const std::string &inFilename,
                                   ProgressCallback progress)
{
    return impl_->decodeFromFile(outImage, inFilename, progress);
}
