#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPoint>
#include <QList>
#include <QMediaPlayer>

// 前向声明
class AudioPlayer;
class LocalMusicManager;
class MusicApiManager;
class HeaderWidget;
class FooterWidget;
class MenuWidget;
class DiscoveryPage;
class LocalMusicPage;
class FavoritePage;
class PlaylistPage;
class DailyRecommendPage;
class RankingPage;
class PlayerDetailPage;
class SearchResultPage;
class MVPage;
class VideoPlayerWidget;
class PlaylistWidget;

// 需要完整定义（因为使用了值类型）
#include "core/musicapimanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onMenuChanged(int index);
    void onBackClicked();
    void onSongDoubleClicked(int index);
    void onNetworkSongDoubleClicked(const SongItem &song);
    void onSongUrlReceived(const QString &songId, const QString &url);
    
    // Player Detail
    void onAlbumCoverClicked();
    
    // Search
    void onSearch(const QString &keyword);

    // New Features
    void onLikeClicked(bool checked);
    void onPlayAllSongs(const QList<SongItem> &songs);
    void onPlaylistClicked();
    void onPlaylistSongClicked(int index);
    void onPlaylistClearClicked();

    void onPlayClicked();
    void onPauseClicked();
    void onPrevClicked();
    void onNextClicked();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    
    // Video Player
    void onVideoClicked(const QString &path);
    void onVideoCloseRequested();

private:
    void setupUI();
    void setupConnections();
    void updateBackButtonState();
    void playSongByIndex(int index);
    void playNetworkSong(const SongItem &song, const QString &url);
    void updateLikeButtonState(const SongItem &song);

    /* ========= 核心模块 ========= */
    AudioPlayer *m_audioPlayer;
    LocalMusicManager *m_musicManager;
    MusicApiManager *m_apiManager;

    /* ========= UI组件 ========= */
    QStackedWidget *m_stackedWidget;
    HeaderWidget *m_headerWidget;
    FooterWidget *m_footerWidget;
    MenuWidget *m_menuWidget;
    PlaylistWidget *m_playlistWidget;

    /* ========= 页面 ========= */
    DiscoveryPage *m_discoveryPage;
    LocalMusicPage *m_localMusicPage;
    FavoritePage *m_favoritePage;
    PlaylistPage *m_playlistPage;
    DailyRecommendPage *m_dailyRecommendPage;
    RankingPage *m_rankingPage;
    PlayerDetailPage *m_playerDetailPage;
    SearchResultPage *m_searchResultPage;
    MVPage *m_mvPage;

    /* ========= 组件 ========= */
    VideoPlayerWidget *m_videoPlayerWidget;


    /* ========= 导航历史 ========= */
    QList<QString> m_navigationHistory;

    /* ========= 当前播放 ========= */
    int m_currentIndex;
    QList<SongItem> m_playlist;  // 统一播放列表
    int m_currentPlayIndex;      // 当前播放索引 (在m_playlist中)
    
    SongItem m_currentNetworkSong;  // 当前要播放的网络歌曲

    /* ========= 窗口拖拽 ========= */
    QPoint m_dragPosition;
    const QSize m_minSize = QSize(1000, 670);
    const QSize m_maxSize = QSize(1600, 1072);
};

#endif // MAINWINDOW_H
