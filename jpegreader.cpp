#include "jpegreader.h"

#include "jpegreaderimplementation.h"

QImage JpegReader::read(const QString &filePath, bool &hasError, QString &errorMessage)
{
    hasError = false;
    JpegData jpgData = loadJpegData(filePath, hasError, errorMessage);
    if (hasError)
        return QImage();
    qDebug() << "Huffman counts " << jpgData.huffmanTables.size();
    for (size_t i = 0; i < jpgData.huffmanTables.size(); ++i)
        getHuffmanMap(jpgData.huffmanTables[i]);
    return QImage();
}

void JpegReader::write(const QImage &image, const QString &filePath, bool &hasError, QString &errorMessage)
{
    hasError = false;
    if (image.size().width() == 0 || image.size().height() == 0)
    {
        hasError = true;
        errorMessage = "Image is empty";
    }
}
