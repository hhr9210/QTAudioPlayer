#ifndef LOCALMUSICMANAGER_H
#define LOCALMUSICMANAGER_H

#include <QObject>
#include <QStringList>
#include "songinfo.h"

class LocalMusicManager : public QObject
{
    Q_OBJECT

public:
    explicit LocalMusicManager(QObject *parent = nullptr);
    ~LocalMusicManager() override;

    void setMusicPath(const QString &path);
    QString musicPath() const { return m_musicPath; }

    void refreshMusicList();
    QList<SongInfo> songs() const { return m_songs; }
    
    QStringList songPaths() const;
    QStringList songNames() const;

    SongInfo getSongInfo(int index) const;
    QString getSongPath(int index) const;
    QString getSongName(int index) const;
    int songCount() const { return m_songs.size(); }

signals:
    void musicListRefreshed();

private:
    SongInfo readSongMetadata(const QString &filePath);

    QString m_musicPath;
    QList<SongInfo> m_songs;
};

#endif // LOCALMUSICMANAGER_H
