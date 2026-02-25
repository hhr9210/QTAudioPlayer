#ifndef SONGINFO_H
#define SONGINFO_H

#include <QString>
#include <QPixmap>
#include <QFileInfo>

struct SongInfo
{
    QString path;           // 文件路径
    QString title;          // 歌曲名
    QString artist;         // 歌手名
    QString album;          // 专辑名
    qint64 duration;        // 时长（毫秒）
    QPixmap cover;          // 专辑封面

    SongInfo()
        : duration(0)
    {}

    SongInfo(const QString &filePath)
        : path(filePath)
        , duration(0)
    {}

    // 格式化时长为 mm:ss
    QString formattedDuration() const
    {
        if (duration <= 0) return "00:00";
        int totalSeconds = duration / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        return QString("%1:%2").arg(minutes, 2, 10, QChar('0'))
                                .arg(seconds, 2, 10, QChar('0'));
    }

    // 获取显示名称（歌手 - 歌曲名）
    QString displayName() const
    {
        if (artist.isEmpty()) {
            return title.isEmpty() ? QFileInfo(path).baseName() : title;
        }
        return artist + " - " + (title.isEmpty() ? QFileInfo(path).baseName() : title);
    }
};

#endif // SONGINFO_H

