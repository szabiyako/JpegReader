#include "jpegreaderimplementationmainutils.h"

using namespace JpegReader::Implementation::MainUtils;

JpegReader::Implementation::MainUtils::bytePair::bytePair(byte first, byte second)
{
    data[0] = first;
    data[1] = second;
}
JpegReader::Implementation::MainUtils::bytePair::bytePair(byte *data)
{
    this->data[0] = data[0];
    this->data[1] = data[1];
}
byte &JpegReader::Implementation::MainUtils::bytePair::operator[](const int index)
{
    return data[index];
}
byte JpegReader::Implementation::MainUtils::bytePair::operator[](const int index) const
{
    return data[index];
}
unsigned int JpegReader::Implementation::MainUtils::bytePair::getSizeBigEndian()
{
    unsigned int size = data[1];
    size += data[0]*256;
    return size;
}

JpegReader::Implementation::MainUtils::BinaryTree::Node::~Node()
{
    if (left != nullptr)
        delete left;
    if (right != nullptr)
        delete right;
}

JpegReader::Implementation::MainUtils::BinaryTree::BinaryTree()
{
    root = new Node;
}

JpegReader::Implementation::MainUtils::BinaryTree::~BinaryTree()
{
    delete root;
}

bool JpegReader::Implementation::MainUtils::JpegData::hasAllParts()
{
    return !appHeader.empty() &&
           !quantizationTables.empty() &&
           !frame.empty() &&
           !huffmanTables.empty() &&
           !scan.empty();
}
