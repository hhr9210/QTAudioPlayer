#ifndef LRCPARSER_H
#define LRCPARSER_H

#include <QString>
#include <QMap>
#include <QList>

struct LrcLine {
    qint64 time; // Milliseconds
    QString text;
};

class LrcParser
{
public:
    static QList<LrcLine> parse(const QString &lrcContent);
    static QList<LrcLine> parseFile(const QString &filePath);
};

#endif // LRCPARSER_H
