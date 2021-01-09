#ifndef JPEGREADERIMPLEMENTATIONMAINUTILS_H
#define JPEGREADERIMPLEMENTATIONMAINUTILS_H

#include <vector>

namespace JpegReader {
namespace Implementation {
namespace MainUtils {

typedef unsigned char byte;

enum DataHeaderType : byte
{
    NOT_A_DATA_HEADER,

    JPEG_START,
    APPLICATION_HEADER,
    COMMENT_HEADER,
    QUANTIZATION_TABLE,
    START_OF_FRAME,
    HUFFMAN_TABLE,
    START_OF_SCAN,
    JPEG_END,

    EXIF_APPLICATION_HEADER
};

struct bytePair
{
private:
    byte data[2];
public:
    bytePair(byte first = 0, byte second = 0);
    bytePair(byte *data);
    byte &operator[](const int index);
    byte operator[](const int index) const;
    unsigned int getSizeBigEndian();
};

struct BinaryTree
{
    struct Node
    {
        bool isLeaf = false;
        byte value = 0x00;
        Node *left = nullptr;
        Node *right = nullptr;
        ~Node();
    };
    Node *root = nullptr;
    BinaryTree();
    ~BinaryTree();
};


struct JpegData
{
    std::vector<byte> appHeader;
    std::vector<std::vector<byte>> comments;
    std::vector<std::vector<byte>> quantizationTables;
    std::vector<byte> frame;
    std::vector<std::vector<byte>> huffmanTables;
    std::vector<byte> scan;
    std::vector<byte> imageData;

    bool hasAllParts();
};

}
}
}


#endif // JPEGREADERIMPLEMENTATIONMAINUTILS_H
