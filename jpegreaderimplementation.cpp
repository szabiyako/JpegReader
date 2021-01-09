#include "jpegreaderimplementation.h"
#include "jpegreaderimplementationmainutils.h"

#include <fstream>
#include <map>
#include <vector>
#include <queue>

using namespace JpegReader::Implementation;
using namespace JpegReader::Implementation::MainUtils;

//TEMP
#include <QDebug>

DataHeaderType JpegReader::Implementation::getHeaderType(const bytePair &bytes)
{
    if (bytes[0] != 0xFF)
        return DataHeaderType::NOT_A_DATA_HEADER;
    switch (bytes[1])
    {
    case 0xD8 : return DataHeaderType::JPEG_START;
    case 0xE0 : return DataHeaderType::APPLICATION_HEADER;
    case 0xFE : return DataHeaderType::COMMENT_HEADER;
    case 0xDB : return DataHeaderType::QUANTIZATION_TABLE;
    case 0xC0 : return DataHeaderType::START_OF_FRAME;
    case 0xC4 : return DataHeaderType::HUFFMAN_TABLE;
    case 0xDA : return DataHeaderType::START_OF_SCAN;
    case 0xD9 : return DataHeaderType::JPEG_END;
    }
    return DataHeaderType::NOT_A_DATA_HEADER;
}

struct BinaryTree
{
    struct Node
    {
        bool isLeaf = false;
        byte value = 0x00;
        Node *left = nullptr;
        Node *right = nullptr;
        ~Node()
        {
            if (left != nullptr)
                delete left;
            if (right != nullptr)
                delete right;
        }
    };
    Node *root = nullptr;
    BinaryTree()
    {
        root = new Node;
    }
    ~BinaryTree()
    {
        delete root;
    }
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

    bool hasAllParts()
    {
        return !appHeader.empty() &&
               !quantizationTables.empty() &&
               !frame.empty() &&
               !huffmanTables.empty() &&
               !scan.empty();
    }
};

void loadData(JpegData &jpgData, std::ifstream &input, DataHeaderType dataTypeToLoad)
{
    bytePair inputSizeOfData;
    input.read((char*)&inputSizeOfData, sizeof(bytePair));
    size_t dataSize = inputSizeOfData.getSizeBigEndian() - 2;
    std::vector<byte> data(dataSize);
    input.read((char*)&data[0], dataSize);
    switch (dataTypeToLoad)
    {
    case DataHeaderType::APPLICATION_HEADER : jpgData.appHeader = data; return;
    case DataHeaderType::COMMENT_HEADER : jpgData.comments.push_back(data); return;
    case DataHeaderType::QUANTIZATION_TABLE : jpgData.quantizationTables.push_back(data); return;
    case DataHeaderType::START_OF_FRAME : jpgData.frame = data; return;
    case DataHeaderType::HUFFMAN_TABLE : jpgData.huffmanTables.push_back(data); return;
    case DataHeaderType::START_OF_SCAN : jpgData.scan = data;
    {
        bytePair inputHeader;
        while (!input.eof())
        {
            byte inputByte;
            input.read((char*)&inputByte, sizeof(byte));
            std::swap(inputHeader[0], inputHeader[1]);
            inputHeader[1] = inputByte;
            jpgData.imageData.push_back(inputByte);
            if (getHeaderType(inputHeader) == DataHeaderType::JPEG_END)
            {
                jpgData.imageData.pop_back();
                jpgData.imageData.pop_back();
                return;
            }
        }
    }
    default: return;
    }
    return;
}

void stepBuildTree(std::queue<BinaryTree::Node*> &nodes,
                   std::queue<byte> &layersCount,
                   std::queue<byte> &values)
{
    int valuesOnThisLayer = layersCount.front();
    layersCount.pop();
    int nodesOnThisLayer = nodes.size();
    for (int index = 0; index < valuesOnThisLayer; ++index)
    {
        BinaryTree::Node *node = nodes.front();
        node->value = values.front();
        node->isLeaf = true;
        nodes.pop();
        values.pop();
    }
    for (int index = valuesOnThisLayer; index < nodesOnThisLayer; ++index)
    {
        BinaryTree::Node *node = nodes.front();
        nodes.push(node->left = new BinaryTree::Node);
        nodes.push(node->right = new BinaryTree::Node);
        nodes.pop();
    }
}

void recursiveFillMap(std::map<std::string, byte> &map, const std::string &code, BinaryTree::Node* currentNode)
{
    if (currentNode == nullptr)
        return;
    if (currentNode->isLeaf)
    {
        map[code] = currentNode->value;
        qDebug() << QString::fromStdString(code) << " = " << currentNode->value; //DELETE
        return;
    }

    recursiveFillMap(map, code + "0", currentNode->left);
    recursiveFillMap(map, code + "1", currentNode->right);
}

std::map<std::string, byte> generateMap(const BinaryTree &tree)
{
    std::map<std::string, byte> map;
    recursiveFillMap(map, "0", tree.root->left);
    recursiveFillMap(map, "1", tree.root->right);
    return map;
}

std::map<std::string, byte> getHuffmanMap(const std::vector<byte> &data)
{
    //Read bytes count for every layer
    std::queue<byte> queuelayerCount;
    for (size_t index = 1; index < 17; ++index)
        queuelayerCount.push(data[index]);

    //Read bytes data
    size_t dataSize = data.size();
    std::queue<byte> queueValues;
    for (size_t index = 17; index < dataSize; ++index)
        queueValues.push(data[index]);
    qDebug() << ((data[0] & 0xF0) >> 4);  //DELETE
    qDebug() << (data[0] & 0x0F);  //DELETE

    //Make a tree
    BinaryTree tree;
    std::queue<BinaryTree::Node*> queueNode;
    queueNode.push(tree.root->left = new BinaryTree::Node);
    queueNode.push(tree.root->right = new BinaryTree::Node);

    while (!queueValues.empty())
        stepBuildTree(queueNode, queuelayerCount, queueValues);
    return generateMap(tree);
}

JpegData loadJpegData(const QString &filePath, bool &hasError, QString &errorMessage)
{
    std::ifstream input(filePath.toStdWString().c_str(), std::ios::binary);
    if (!input.is_open())
    {
        hasError = true;
        errorMessage = "Can't open file \"" + filePath + "\"";
    }

    JpegData jpgData;
    bool foundStart = false;
    while (!input.eof())
    {
        bytePair inputBytes;
        input.read((char*)&inputBytes, sizeof(bytePair));
        DataHeaderType inputHeader = getHeaderType(inputBytes);
        if (inputHeader != DataHeaderType::NOT_A_DATA_HEADER)
        {
            if (inputHeader == DataHeaderType::JPEG_START)
                foundStart = true;
            else
                loadData(jpgData, input, inputHeader);
            if (inputHeader == DataHeaderType::START_OF_SCAN)
                break;
        }
    }
    input.close();
    if (!foundStart)
    {
        hasError = true;
        errorMessage = "Not found jpeg_start bytes";
    }
    if (!jpgData.hasAllParts())
    {
        hasError = true;
        errorMessage = "File is corrupted";
    }
    return jpgData;
}
