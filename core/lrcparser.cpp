#include "lrcparser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

QList<LrcLine> LrcParser::parse(const QString &lrcContent)
{
    QList<LrcLine> lines;
    QStringList rawLines = lrcContent.split('\n');
    

    QRegularExpression timeRegex("\\[(\\d{2}):(\\d{2})\\.(\\d{2,3})\\]");
    
    for (const QString &line : rawLines) {
        QString cleanLine = line.trimmed();
        if (cleanLine.isEmpty()) continue;
        
        QRegularExpressionMatchIterator i = timeRegex.globalMatch(cleanLine);
        
        int lastMatchEnd = 0;
        bool hasTime = false;
        
        // [00:10.00][00:20.00]歌词
        QList<qint64> times;
        
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            hasTime = true;
            
            int minutes = match.captured(1).toInt();
            int seconds = match.captured(2).toInt();
            int millis = match.captured(3).toInt();
            
            // Normalize millis (if 2 digits, it's usually 1/100s)
            if (match.captured(3).length() == 2) {
                millis *= 10;
            }
            
            qint64 totalMillis = (minutes * 60 + seconds) * 1000 + millis;
            times.append(totalMillis);
            
            lastMatchEnd = match.capturedEnd();
        }
        
        if (hasTime) {
            QString text = cleanLine.mid(lastMatchEnd).trimmed();
            for (qint64 t : times) {
                lines.append({t, text});
            }
        }
    }
    
    // 时间排序
    std::sort(lines.begin(), lines.end(), [](const LrcLine &a, const LrcLine &b) {
        return a.time < b.time;
    });
    
    return lines;
}

QList<LrcLine> LrcParser::parseFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    
    QTextStream in(&file);
    // UTF8
    in.setEncoding(QStringConverter::Utf8); 
    QString content = in.readAll();
    file.close();
    
    return parse(content);
}
