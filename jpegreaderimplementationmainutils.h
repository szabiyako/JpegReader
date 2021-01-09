#ifndef JPEGREADERIMPLEMENTATIONMAINUTILS_H
#define JPEGREADERIMPLEMENTATIONMAINUTILS_H

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
    JPEG_END
};

struct bytePair
{
private:
    byte data[2];
public:
    bytePair(byte first = 0, byte second = 0)
    {
        data[0] = first;
        data[1] = second;
    }
    bytePair(byte *data)
    {
        this->data[0] = data[0];
        this->data[1] = data[1];
    }
    byte &operator[](const int index)
    {
        return data[index];
    }
    byte operator[](const int index) const
    {
        return data[index];
    }
    unsigned int getSizeBigEndian()
    {
        unsigned int size = data[1];
        size += data[0]*256;
        return size;
    }
};

}
}
}


#endif // JPEGREADERIMPLEMENTATIONMAINUTILS_H
