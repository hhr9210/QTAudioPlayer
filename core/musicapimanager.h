#ifndef MUSICAPIMANAGER_H
#define MUSICAPIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

struct SongItem
{
    QString id;
    QString name;
    QString artist;
    QString album;
    QString picUrl;
    QString playUrl;
    qint64 duration;
    QPixmap coverImage;
    
    SongItem() : duration(0) {}
};

class MusicApiManager : public QObject
{
    Q_OBJECT

public:
    explicit MusicApiManager(QObject *parent = nullptr);
    ~MusicApiManager() override;

    // 获取推荐歌曲列表
    void fetchRecommendSongs(int limit = 20);
    
    // 搜索歌曲
    void searchSongs(const QString &keyword, int limit = 20);
    
    // 获取热门歌曲（使用真实API）
    void fetchHotSongs(int limit = 20);
    
    // 根据歌曲ID获取播放URL
    void fetchSongUrl(const QString &songId);

signals:
    void recommendSongsReceived(const QList<SongItem> &songs);
    void searchResultsReceived(const QList<SongItem> &songs);
    void hotSongsReceived(const QList<SongItem> &songs);
    void songUrlReceived(const QString &songId, const QString &url);
    void errorOccurred(const QString &error);

private slots:
    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    enum ApiRequestType {
        RequestHotSongs,
        RequestRecommendSongs,
        RequestSearch,
        RequestSongUrl
    };
    
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    ApiRequestType m_currentRequestType;
    int m_requestLimit;
    QString m_currentSongId;  // 当前请求的歌曲ID
    
    void parseSongsFromJson(const QJsonArray &songsArray, QList<SongItem> &songs);
    void parseHotSongsFromJson(const QJsonDocument &doc, QList<SongItem> &songs);
    QString formatDuration(qint64 duration);
    
    // 本地歌曲相关
    QList<SongItem> m_localSongs;
    void loadLocalSongs();
};

#endif // MUSICAPIMANAGER_H

