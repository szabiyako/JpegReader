#ifndef JPEGREADER_H
#define JPEGREADER_H

#include <QImage>
#include <QString>

namespace JpegReader {
    QImage read(const QString &filePath, bool &hasError, QString &errorMessage);
    void write(const QImage &image, const QString &filePath, bool &hasError, QString &errorMessage);
}

#endif // JPEGREADER_H
