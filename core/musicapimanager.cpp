#include "musicapimanager.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <algorithm>
#include <random>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QEventLoop>
#include <QImage>

MusicApiManager::MusicApiManager(QObject *parent)
    : QObject(parent)
    , m_currentReply(nullptr)
    , m_currentRequestType(RequestHotSongs)
    , m_requestLimit(20)
{
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &MusicApiManager::onNetworkReplyFinished);
    
    // 加载本地歌曲
    loadLocalSongs();
}

MusicApiManager::~MusicApiManager() = default;

void MusicApiManager::loadLocalSongs()
{
    m_localSongs.clear();
    
    // 假设Songs文件夹在应用程序运行目录
    QString songsPath = "E:/qttest/mp/Songs";
    QDir dir(songsPath);
    
    if (!dir.exists()) {
        qDebug() << "Songs文件夹不存在:" << dir.absolutePath();
        return;
    }
    
    QStringList filters;
    filters << "*.mp3" << "*.flac" << "*.wav" << "*.m4a";
    dir.setNameFilters(filters);
    
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    
    int idCounter = 1;
    
    for (const QFileInfo &fileInfo : fileList) {
        SongItem song;
        song.id = QString::number(idCounter++);
        song.playUrl = fileInfo.absoluteFilePath();
        
        // 1. 基础信息从文件名获取 (作为备份)
        QString baseName = fileInfo.completeBaseName();
        QStringList parts = baseName.split(" - ");
        if (parts.size() >= 2) {
            song.artist = parts.first().trimmed();
            parts.removeFirst();
            song.name = parts.join(" - ").trimmed();
        } else {
            song.name = baseName;
            song.artist = "未知艺术家";
        }
        song.album = "本地音乐";
        song.duration = 240000; // 默认
        
        // 2. 尝试读取元数据 (封面, 真实时长, 准确信息)
        QMediaPlayer *player = new QMediaPlayer(this);
        player->setSource(QUrl::fromLocalFile(song.playUrl));
        
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(1000); // 1s timeout for better cover extraction
        
        connect(player, &QMediaPlayer::mediaStatusChanged, &loop, [&](QMediaPlayer::MediaStatus status){
            if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
                loop.quit();
            }
        });
        connect(player, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        
        timer.start();
        loop.exec();
        
        // 读取信息
        QVariant titleVar = player->metaData().value(QMediaMetaData::Title);
        if (titleVar.isValid() && !titleVar.toString().isEmpty()) {
            song.name = titleVar.toString();
        }
        
        QVariant artistVar = player->metaData().value(QMediaMetaData::AlbumArtist);
        if (!artistVar.isValid() || artistVar.toString().isEmpty()) {
            artistVar = player->metaData().value(QMediaMetaData::ContributingArtist);
        }
        if (artistVar.isValid() && !artistVar.toString().isEmpty()) {
            song.artist = artistVar.toString();
        }
        
        QVariant albumVar = player->metaData().value(QMediaMetaData::AlbumTitle);
        if (albumVar.isValid() && !albumVar.toString().isEmpty()) {
            song.album = albumVar.toString();
        }
        
        if (player->duration() > 0) {
            song.duration = player->duration();
        }
        
        QVariant coverVar = player->metaData().value(QMediaMetaData::CoverArtImage);
        if (!coverVar.isValid()) {
            coverVar = player->metaData().value(QMediaMetaData::ThumbnailImage);
        }
        if (coverVar.isValid()) {
            QImage img = coverVar.value<QImage>();
            if (!img.isNull()) {
                song.coverImage = QPixmap::fromImage(img);
            }
        }
        
        player->deleteLater();
        
        m_localSongs.append(song);
    }
    
    qDebug() << "加载了" << m_localSongs.size() << "首本地歌曲";
}

void MusicApiManager::fetchRecommendSongs(int limit)
{
    m_currentRequestType = RequestRecommendSongs;
    
    // 从本地歌曲中随机选择
    QList<SongItem> result;
    if (m_localSongs.isEmpty()) {
        QTimer::singleShot(100, this, [this]() {
            emit recommendSongsReceived(QList<SongItem>());
        });
        return;
    }
    
    QList<SongItem> shuffled = m_localSongs;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffled.begin(), shuffled.end(), g);
    
    for (int i = 0; i < limit && i < shuffled.size(); ++i) {
        result.append(shuffled[i]);
    }
    
    QTimer::singleShot(200, this, [this, result]() {
        emit recommendSongsReceived(result);
    });
}

void MusicApiManager::searchSongs(const QString &keyword, int limit)
{
    m_currentRequestType = RequestSearch;
    m_requestLimit = limit;
    
    QList<SongItem> result;
    for (const SongItem &song : m_localSongs) {
        if (song.name.contains(keyword, Qt::CaseInsensitive) || 
            song.artist.contains(keyword, Qt::CaseInsensitive)) {
            result.append(song);
            if (result.size() >= limit) break;
        }
    }
    
    QTimer::singleShot(200, this, [this, result]() {
        emit searchResultsReceived(result);
    });
}

void MusicApiManager::fetchHotSongs(int limit)
{
    m_currentRequestType = RequestHotSongs;
    m_requestLimit = limit;
    
    // 直接返回本地歌曲列表作为热歌榜
    QList<SongItem> result;
    for (int i = 0; i < limit && i < m_localSongs.size(); ++i) {
        result.append(m_localSongs[i]);
    }
    
    QTimer::singleShot(200, this, [this, result]() {
        emit hotSongsReceived(result);
    });
}

void MusicApiManager::fetchSongUrl(const QString &songId)
{
    m_currentRequestType = RequestSongUrl;
    m_currentSongId = songId;
    
    // 查找对应ID的歌曲
    QString url;
    for (const SongItem &song : m_localSongs) {
        if (song.id == songId) {
            url = song.playUrl;
            break;
        }
    }
    
    if (!url.isEmpty()) {
        QTimer::singleShot(50, this, [this, songId, url]() {
            emit songUrlReceived(songId, url);
        });
    } else {
        QTimer::singleShot(50, this, [this]() {
            emit errorOccurred("未找到该歌曲文件");
        });
    }
}

void MusicApiManager::onNetworkReplyFinished(QNetworkReply *reply)
{
    // 不再处理网络请求
    if (reply) reply->deleteLater();
}

void MusicApiManager::parseSongsFromJson(const QJsonArray &songsArray, QList<SongItem> &songs)
{
    // 不再使用
    Q_UNUSED(songsArray);
    Q_UNUSED(songs);
}

void MusicApiManager::parseHotSongsFromJson(const QJsonDocument &doc, QList<SongItem> &songs)
{
    // 不再使用
    Q_UNUSED(doc);
    Q_UNUSED(songs);
}

QString MusicApiManager::formatDuration(qint64 duration)
{
    int totalSeconds = duration / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));
}
