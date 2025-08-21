#include "Block4Strategy.h"

#include "BitWriter.h"
#include "BitReader.h"

std::vector<uint8_t> Block4Strategy::encodeRow(const std::vector<uint8_t>& row) {
    BitWriter writer;

    for (size_t i = 0; i < row.size(); i += 4) {
        uint8_t b[4] = {0xFF,0xFF,0xFF,0xFF};

        for (size_t j = 0; j < 4 && (i + j) < row.size(); ++j) {
            b[j] = row[i+j];
        }

        // Всі білі
        if (b[0]==0xFF && b[1]==0xFF && b[2]==0xFF && b[3]==0xFF) {
            writer.writeBits(0b0, 1);   // "0"
        }
        // Всі чорні
        else if (b[0]==0x00 && b[1]==0x00 && b[2]==0x00 && b[3]==0x00) {
            writer.writeBits(0b10, 2);  // "10"
        }
        // Змішаний блок
        else {
            writer.writeBits(0b11, 2);  // "11"
            for (int j=0;j<4;++j) {
                writer.writeBits(b[j], 8);
            }
        }
    }

    return writer.getBytes();
}

std::vector<uint8_t> Block4Strategy::decodeRow(BitReader &reader, size_t rowLength) {
    auto result = decodeRowWithBitCount(reader, rowLength);
    return result.first;
}

std::pair<std::vector<uint8_t>, size_t> Block4Strategy::decodeRowWithBitCount(BitReader &reader, size_t rowLength) {
    std::vector<uint8_t> row;
    row.reserve(rowLength);
    size_t bitsRead = 0;

    while (row.size() < rowLength) {
        uint32_t firstBit = reader.readBits(1);
        bitsRead += 1;

        if (firstBit == 0b0) {
            // 4 білих пікселі
            for (int i = 0; i < 4 && row.size() < rowLength; ++i) {
                row.push_back(0xFF);
            }
        } else {
            // Другий біт визначає тип
            uint32_t secondBit = reader.readBits(1);
            bitsRead += 1;

            if (secondBit == 0b0) {
                // 4 чорних пікселі
                for (int i = 0; i < 4 && row.size() < rowLength; ++i) {
                    row.push_back(0x00);
                }
            } else {
                // Змішаний блок
                for (int i = 0; i < 4 && row.size() < rowLength; ++i) {
                    row.push_back(static_cast<uint8_t>(reader.readBits(8)));
                }
                bitsRead += 32;
            }
        }
    }

    return {row, bitsRead};
}
