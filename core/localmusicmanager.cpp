#include "localmusicmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QImage>
#include <QUrl>
#include <QEventLoop>
#include <QTimer>

LocalMusicManager::LocalMusicManager(QObject *parent)
    : QObject(parent)
    , m_musicPath("E:/CloudMusic")
{
}

LocalMusicManager::~LocalMusicManager() = default;

void LocalMusicManager::setMusicPath(const QString &path)
{
    m_musicPath = path;
}

SongInfo LocalMusicManager::readSongMetadata(const QString &filePath)
{
    SongInfo info(filePath);
    QFileInfo fileInfo(filePath);
    
    // 默认使用文件名作为标题
    info.title = fileInfo.baseName();
    
    // 使用 QMediaPlayer 读取元数据
    QMediaPlayer *player = new QMediaPlayer(this);
    player->setSource(QUrl::fromLocalFile(filePath));
    
    // 等待元数据加载（异步操作，需要等待）
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(1000); // 最多等待1秒
    
    connect(player, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    timer.start();
    loop.exec();
    
    // 读取元数据（Qt6 中直接读取，无效时会返回无效的 QVariant）
    // 标题
    QVariant titleVar = player->metaData().value(QMediaMetaData::Title);
    if (titleVar.isValid() && !titleVar.toString().isEmpty()) {
        info.title = titleVar.toString();
    }
    
    // 歌手
    QVariant artistVar = player->metaData().value(QMediaMetaData::AlbumArtist);
    if (!artistVar.isValid() || artistVar.toString().isEmpty()) {
        artistVar = player->metaData().value(QMediaMetaData::ContributingArtist);
    }
    if (artistVar.isValid() && !artistVar.toString().isEmpty()) {
        info.artist = artistVar.toString();
    }
    
    // 专辑
    QVariant albumVar = player->metaData().value(QMediaMetaData::AlbumTitle);
    if (albumVar.isValid() && !albumVar.toString().isEmpty()) {
        info.album = albumVar.toString();
    }
    
    // 时长
    if (player->duration() > 0) {
        info.duration = player->duration();
    }
    
    // 封面
    QVariant coverVar = player->metaData().value(QMediaMetaData::CoverArtImage);
    if (!coverVar.isValid()) {
        coverVar = player->metaData().value(QMediaMetaData::ThumbnailImage);
    }
    if (coverVar.isValid()) {
        QImage coverImage = coverVar.value<QImage>();
        if (!coverImage.isNull()) {
            info.cover = QPixmap::fromImage(coverImage);
        }
    }
    
    // 如果时长还是0，尝试从duration元数据获取
    if (info.duration <= 0) {
        QVariant durationVar = player->metaData().value(QMediaMetaData::Duration);
        if (durationVar.isValid()) {
            info.duration = durationVar.toLongLong();
        }
    }
    
    player->deleteLater();
    
    return info;
}

void LocalMusicManager::refreshMusicList()
{
    m_songs.clear();

    QDir dir(m_musicPath);
    if (!dir.exists()) {
        emit musicListRefreshed();
        return;
    }

    QStringList files = dir.entryList({"*.mp3", "*.wav", "*.flac", "*.m4a"}, QDir::Files);

    for (const QString &file : files) {
        QString path = dir.absoluteFilePath(file);
        SongInfo info = readSongMetadata(path);
        m_songs << info;
    }

    emit musicListRefreshed();
}

QStringList LocalMusicManager::songPaths() const
{
    QStringList paths;
    for (const SongInfo &info : m_songs) {
        paths << info.path;
    }
    return paths;
}

QStringList LocalMusicManager::songNames() const
{
    QStringList names;
    for (const SongInfo &info : m_songs) {
        names << info.displayName();
    }
    return names;
}

SongInfo LocalMusicManager::getSongInfo(int index) const
{
    if (index >= 0 && index < m_songs.size()) {
        return m_songs[index];
    }
    return SongInfo();
}

QString LocalMusicManager::getSongPath(int index) const
{
    if (index >= 0 && index < m_songs.size()) {
        return m_songs[index].path;
    }
    return QString();
}

QString LocalMusicManager::getSongName(int index) const
{
    if (index >= 0 && index < m_songs.size()) {
        return m_songs[index].displayName();
    }
    return QString();
}
