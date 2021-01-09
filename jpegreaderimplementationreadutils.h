#ifndef JPEGREADERIMPLEMENTATIONREADUTILS_H
#define JPEGREADERIMPLEMENTATIONREADUTILS_H

#include <fstream>
#include <queue>
#include <map>
#include <QString>

#include "jpegreaderimplementationmainutils.h"

using namespace JpegReader::Implementation::MainUtils;

namespace JpegReader {
namespace Implementation {
namespace ReadUtils {

DataHeaderType getHeaderType(const bytePair &bytes);

void loadData(JpegData &jpgData, std::ifstream &input, DataHeaderType dataTypeToLoad);

void stepBuildTree(std::queue<BinaryTree::Node*> &nodes,
                   std::queue<byte> &layersCount,
                   std::queue<byte> &values);

void recursiveFillMap(std::map<std::string, byte> &map, const std::string &code, BinaryTree::Node* currentNode);

std::map<std::string, byte> generateMap(const BinaryTree &tree);

std::map<std::string, byte> getHuffmanMap(const std::vector<byte> &data);

JpegData loadJpegData(const QString &filePath, bool &hasError, QString &errorMessage);

}
}
}


#endif // JPEGREADERIMPLEMENTATIONREADUTILS_H
