#include "mainwindow.h"

#include "core/audioplayer.h"
#include "core/localmusicmanager.h"
#include "core/songinfo.h"
#include "core/musicapimanager.h"
#include "widgets/headerwidget.h"
#include "widgets/footerwidget.h"
#include "widgets/menuwidget.h"
#include "pages/discoverypage.h"
#include "pages/localmusicpage.h"
#include "pages/mvpage.h"
#include "pages/favoritepage.h"
#include "pages/playlistpage.h"
#include "pages/dailyrecommendpage.h"
#include "pages/rankingpage.h"
#include "pages/playerdetailpage.h"
#include "pages/searchresultpage.h"
#include "widgets/playlistwidget.h"
#include "widgets/videoplayerwidget.h"

#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QUrl>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QResizeEvent>
#include <QCryptographicHash>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentPlayIndex(-1)
{
    // 创建核心模块
    m_audioPlayer = new AudioPlayer(this);
    m_musicManager = new LocalMusicManager(this);
    m_apiManager = new MusicApiManager(this);

    setupUI();
    setupConnections();

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setMinimumSize(m_minSize);
    setMaximumSize(m_maxSize);
    resize(1000, 670);

    setMouseTracking(true);
    centralWidget()->setMouseTracking(true);

    // 初始化导航历史
    m_navigationHistory.append("发现音乐");
    updateBackButtonState();

    // 设置默认选中发现音乐
    m_menuWidget->setCurrentRow(0);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(10, 10, 10, 10);
    rootLayout->setSpacing(0);

    QFrame *mainFrame = new QFrame();
    mainFrame->setObjectName("MainFrame");
    mainFrame->setStyleSheet(
        "QFrame#MainFrame {"
        "   background-color: #ffffff;"
        "   border: 1px solid #dcdcdc;"
        "   border-radius: 8px;"
        "}"
        "QWidget { color: #333333; }"
        "QScrollArea { background-color: transparent; border: none; }"
        "QScrollBar:vertical { border: none; background: #ffffff; width: 10px; margin: 0; }"
        "QScrollBar::handle:vertical { background: #e1e1e1; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::handle:vertical:hover { background: #cfcfcf; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar:horizontal { border: none; background: #ffffff; height: 10px; margin: 0; }"
        "QScrollBar::handle:horizontal { background: #e1e1e1; min-width: 20px; border-radius: 5px; }"
        "QScrollBar::handle:horizontal:hover { background: #cfcfcf; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
    );
    rootLayout->addWidget(mainFrame);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    mainFrame->setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Header
    m_headerWidget = new HeaderWidget(this);
    mainLayout->addWidget(m_headerWidget);

    // Middle
    QWidget *middleWidget = new QWidget();
    QHBoxLayout *middle = new QHBoxLayout(middleWidget);
    middle->setContentsMargins(0, 0, 0, 0);
    middle->setSpacing(0);

    m_menuWidget = new MenuWidget();
    middle->addWidget(m_menuWidget, 0);

    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stackedWidget->setContentsMargins(0, 0, 0, 0);

    // 创建所有页面
    m_discoveryPage = new DiscoveryPage();
    m_localMusicPage = new LocalMusicPage(m_musicManager);
    m_mvPage = new MVPage();
    m_favoritePage = new FavoritePage();
    m_playlistPage = new PlaylistPage();
    m_dailyRecommendPage = new DailyRecommendPage();
    m_rankingPage = new RankingPage();
    m_playerDetailPage = new PlayerDetailPage();
    m_searchResultPage = new SearchResultPage();

    m_stackedWidget->addWidget(m_discoveryPage);        // 0: 发现音乐
    m_stackedWidget->addWidget(m_localMusicPage);       // 1: 本地音乐
    m_stackedWidget->addWidget(m_mvPage);              // 2: MV
    m_stackedWidget->addWidget(m_favoritePage);        // 3: 我喜欢的音乐
    m_stackedWidget->addWidget(m_playlistPage);        // 4: 创建的歌单
    m_stackedWidget->addWidget(m_dailyRecommendPage);  // 5: 每日推荐
    m_stackedWidget->addWidget(m_rankingPage);         // 6: 排行榜
    m_stackedWidget->addWidget(m_playerDetailPage);     // 7: 播放详情页
    m_stackedWidget->addWidget(m_searchResultPage);     // 8: 搜索结果页

    middle->addWidget(m_stackedWidget, 1);
    mainLayout->addWidget(middleWidget, 1);

    // Footer
    m_footerWidget = new FooterWidget(m_audioPlayer, this);
    mainLayout->addWidget(m_footerWidget);

    // Playlist Widget
    m_playlistWidget = new PlaylistWidget(this);
    m_playlistWidget->hide();

    // Video Player Widget (Fullscreen overlay)
    m_videoPlayerWidget = new VideoPlayerWidget(this);
    m_videoPlayerWidget->hide();
}

void MainWindow::setupConnections()
{
    // 菜单切换
    connect(m_menuWidget, &MenuWidget::currentRowChanged, this, &MainWindow::onMenuChanged);

    // 后退按钮
    connect(m_headerWidget, &HeaderWidget::backClicked, this, &MainWindow::onBackClicked);
    
    // 搜索
    connect(m_headerWidget, &HeaderWidget::searchRequested, this, &MainWindow::onSearch);
    connect(m_apiManager, &MusicApiManager::searchResultsReceived, m_searchResultPage, &SearchResultPage::setSearchResults);
    connect(m_searchResultPage, &SearchResultPage::songDoubleClicked, this, &MainWindow::onNetworkSongDoubleClicked);

    // 本地音乐双击播放
    connect(m_localMusicPage, &LocalMusicPage::songDoubleClicked, this, &MainWindow::onSongDoubleClicked);
    
    // 网络歌曲双击播放
    connect(m_discoveryPage, &DiscoveryPage::networkSongDoubleClicked, this, &MainWindow::onNetworkSongDoubleClicked);
    
    // 收藏页播放
    connect(m_favoritePage, &FavoritePage::songDoubleClicked, this, &MainWindow::onNetworkSongDoubleClicked);
    connect(m_favoritePage, &FavoritePage::playAllRequested, this, &MainWindow::onPlayAllSongs);

    // 歌曲URL获取完成
    connect(m_apiManager, &MusicApiManager::songUrlReceived, this, &MainWindow::onSongUrlReceived);
    connect(m_apiManager, &MusicApiManager::errorOccurred, this, [](const QString &error) {
        qDebug() << "API错误:" << error;
    });

    // 播放控制
    connect(m_footerWidget, &FooterWidget::playClicked, this, &MainWindow::onPlayClicked);
    connect(m_footerWidget, &FooterWidget::pauseClicked, this, &MainWindow::onPauseClicked);
    connect(m_footerWidget, &FooterWidget::prevClicked, this, &MainWindow::onPrevClicked);
    connect(m_footerWidget, &FooterWidget::nextClicked, this, &MainWindow::onNextClicked);
    connect(m_footerWidget, &FooterWidget::albumCoverClicked, this, &MainWindow::onAlbumCoverClicked);
    connect(m_footerWidget, &FooterWidget::likeClicked, this, &MainWindow::onLikeClicked);
    connect(m_footerWidget, &FooterWidget::playlistClicked, this, &MainWindow::onPlaylistClicked);

    // Playlist Widget
    connect(m_playlistWidget, &PlaylistWidget::songClicked, this, &MainWindow::onPlaylistSongClicked);
    connect(m_playlistWidget, &PlaylistWidget::clearClicked, this, &MainWindow::onPlaylistClearClicked);

    // 媒体状态变化（自动播放下一首）
    connect(m_audioPlayer, &AudioPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    
    // Player Detail Page Sync
    connect(m_audioPlayer, &AudioPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state){
        if (m_playerDetailPage) {
            m_playerDetailPage->setPlaybackState(state == QMediaPlayer::PlayingState);
        }
    });

    // Sync position for lyrics
    connect(m_audioPlayer, &AudioPlayer::positionChanged, this, [this](qint64 position){
        if (m_playerDetailPage) {
            m_playerDetailPage->updatePosition(position);
        }
    });

    // Player Detail Page Close Button
    connect(m_playerDetailPage, &PlayerDetailPage::backClicked, this, &MainWindow::onBackClicked);

    // Video Player
    connect(m_mvPage, &MVPage::videoClicked, this, &MainWindow::onVideoClicked);
    connect(m_videoPlayerWidget, &VideoPlayerWidget::closeRequested, this, &MainWindow::onVideoCloseRequested);
}

void MainWindow::onMenuChanged(int index)
{
    if (index < 0 || index >= m_menuWidget->count()) return;

    QString currentText = m_menuWidget->item(index)->text();

    // 更新导航历史
    if (!m_navigationHistory.isEmpty() && m_navigationHistory.last() != currentText) {
        m_navigationHistory.append(currentText);
    } else if (m_navigationHistory.isEmpty()) {
        m_navigationHistory.append(currentText);
    }

    // 切换页面
    m_stackedWidget->setCurrentIndex(index);

    // 更新后退按钮状态
    updateBackButtonState();
}

void MainWindow::onBackClicked()
{
    if (m_navigationHistory.size() > 1) {
        // 移除当前页面
        m_navigationHistory.removeLast();

        // 获取上一个页面
        QString prevPage = m_navigationHistory.last();

        if (prevPage == "PlayerDetail") {
            m_menuWidget->hide();
            m_stackedWidget->setCurrentWidget(m_playerDetailPage);
        } else {
            m_menuWidget->show();
            // 找到对应的索引并切换
            for (int i = 0; i < m_menuWidget->count(); ++i) {
                if (m_menuWidget->item(i)->text() == prevPage) {
                    m_menuWidget->blockSignals(true);
                    m_menuWidget->setCurrentRow(i);
                    m_menuWidget->blockSignals(false);
                    m_stackedWidget->setCurrentIndex(i);
                    break;
                }
            }
        }
        updateBackButtonState();
    }
}

void MainWindow::onSearch(const QString &keyword)
{
    m_apiManager->searchSongs(keyword);

    // Switch to search page
    m_menuWidget->show();
    m_menuWidget->blockSignals(true);
    m_menuWidget->setCurrentRow(-1);
    m_menuWidget->blockSignals(false);
    m_stackedWidget->setCurrentWidget(m_searchResultPage);
    
    // Update history
    if (m_navigationHistory.isEmpty() || m_navigationHistory.last() != "Search") {
        m_navigationHistory.append("Search");
    }
    updateBackButtonState();
}

void MainWindow::onSongDoubleClicked(int index)
{
    // 本地音乐双击，替换播放列表为本地音乐列表
    m_playlist.clear();
    for(int i=0; i<m_musicManager->songCount(); ++i) {
        SongInfo info = m_musicManager->getSongInfo(i);
        SongItem item;
        // Generate ID from path since SongInfo has no ID
        item.id = QCryptographicHash::hash(info.path.toUtf8(), QCryptographicHash::Md5).toHex();
        item.name = info.title;
        item.artist = info.artist;
        item.album = info.album;
        item.playUrl = QUrl::fromLocalFile(info.path).toString();
        item.duration = info.duration;
        item.coverImage = info.cover; // Local cover
        m_playlist.append(item);
    }
    
    // 播放指定索引
    if (index >= 0 && index < m_playlist.size()) {
        m_currentPlayIndex = index;
        m_playlistWidget->setPlaylist(m_playlist, m_currentPlayIndex);
        
        const SongItem &song = m_playlist[m_currentPlayIndex];
        m_audioPlayer->setSource(QUrl(song.playUrl));
        m_audioPlayer->play();
        
        // Update UI
        SongInfo info = m_musicManager->getSongInfo(index);
        m_footerWidget->setSongInfo(info.title, info.artist, info.album, info.cover);
        
        updateLikeButtonState(song);
        
        if (m_playerDetailPage) {
            m_playerDetailPage->setSongInfo(info.title, info.artist, info.cover);
            
            // Try to load lyrics
            QString lrcPath = info.path;
            int lastDot = lrcPath.lastIndexOf('.');
            if (lastDot != -1) {
                lrcPath = lrcPath.left(lastDot) + ".lrc";
                m_playerDetailPage->loadLyrics(lrcPath);
            }
        }
    }
}

void MainWindow::onNetworkSongDoubleClicked(const SongItem &song)
{
    // 网络歌曲双击，添加到播放列表并播放
    
    int index = -1;
    for(int i=0; i<m_playlist.size(); ++i) {
        if (m_playlist[i].id == song.id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        m_playlist.append(song);
        index = m_playlist.size() - 1;
        m_playlistWidget->setPlaylist(m_playlist, index);
    } else {
        m_playlistWidget->updateCurrentIndex(index);
    }
    
    m_currentPlayIndex = index;
    m_currentNetworkSong = song; // Keep track for URL fetching
    
    // 如果有播放URL，直接播放
    if (!song.playUrl.isEmpty()) {
        playNetworkSong(song, song.playUrl);
    } 
    // 如果没有播放URL，但有歌曲ID，调用API获取
    else if (!song.id.isEmpty()) {
        qDebug() << "获取歌曲播放URL，ID:" << song.id;
        m_apiManager->fetchSongUrl(song.id);
    } 
    else {
        qDebug() << "无法播放: 歌曲缺少ID和播放URL";
    }
}

void MainWindow::onSongUrlReceived(const QString &songId, const QString &url)
{
    qDebug() << "收到播放URL:" << url << "歌曲ID:" << songId;
    
    // 更新播放列表中对应歌曲的URL
    for(int i=0; i<m_playlist.size(); ++i) {
        if (m_playlist[i].id == songId) {
            m_playlist[i].playUrl = url;
            break;
        }
    }
    
    // 检查是否是当前要播放的歌曲
    if (m_currentNetworkSong.id == songId && !url.isEmpty()) {
        // 更新当前网络歌曲对象的URL
        m_currentNetworkSong.playUrl = url;
        playNetworkSong(m_currentNetworkSong, url);
    }
}

void MainWindow::playNetworkSong(const SongItem &song, const QString &url)
{
    qDebug() << "播放网络歌曲:" << song.name << "URL:" << url;
    
    // 设置播放源并播放
    m_audioPlayer->setSource(QUrl(url));
    m_audioPlayer->play();
    
    // 更新UI
    m_footerWidget->setSongInfo(song.name, song.artist, song.album, song.coverImage);
    
    updateLikeButtonState(song);
    
    if (m_playerDetailPage) {
        m_playerDetailPage->setSongInfo(song.name, song.artist, song.coverImage);
        
        // Try to load lyrics from local file path if available
        // Note: For real network songs, we would fetch lrc from URL
        QString lrcPath = song.playUrl;
        int lastDot = lrcPath.lastIndexOf('.');
        if (lastDot != -1) {
             lrcPath = lrcPath.left(lastDot) + ".lrc";
             m_playerDetailPage->loadLyrics(lrcPath);
        }
    }
    
    // 更新播放列表选中状态
    if (m_currentPlayIndex >= 0 && m_currentPlayIndex < m_playlist.size()) {
        m_playlistWidget->updateCurrentIndex(m_currentPlayIndex);
    }
}

void MainWindow::onPlayClicked()
{
    m_audioPlayer->play();
}

void MainWindow::onPauseClicked()
{
    m_audioPlayer->pause();
}

void MainWindow::onPrevClicked()
{
    if (m_playlist.isEmpty()) return;
    
    int newIndex = m_currentPlayIndex - 1;
    if (newIndex < 0) {
        newIndex = m_playlist.size() - 1;
    }
    
    m_currentPlayIndex = newIndex;
    const SongItem &song = m_playlist[m_currentPlayIndex];
    
    if (song.playUrl.isEmpty() && !song.id.isEmpty()) {
        m_currentNetworkSong = song;
        m_apiManager->fetchSongUrl(song.id);
        m_playlistWidget->updateCurrentIndex(m_currentPlayIndex);
        m_footerWidget->setSongInfo(song.name, song.artist, song.album, song.coverImage);
    } else {
        playNetworkSong(song, song.playUrl);
    }
}

void MainWindow::onNextClicked()
{
    if (m_playlist.isEmpty()) return;
    
    int newIndex = m_currentPlayIndex + 1;
    if (newIndex >= m_playlist.size()) {
        newIndex = 0;
    }
    
    m_currentPlayIndex = newIndex;
    const SongItem &song = m_playlist[m_currentPlayIndex];
    
    if (song.playUrl.isEmpty() && !song.id.isEmpty()) {
        m_currentNetworkSong = song;
        m_apiManager->fetchSongUrl(song.id);
        m_playlistWidget->updateCurrentIndex(m_currentPlayIndex);
        m_footerWidget->setSongInfo(song.name, song.artist, song.album, song.coverImage);
    } else {
        playNetworkSong(song, song.playUrl);
    }
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        onNextClicked();
    }
}

void MainWindow::onLikeClicked(bool checked)
{
    if (m_currentPlayIndex < 0 || m_currentPlayIndex >= m_playlist.size()) return;
    
    const SongItem &song = m_playlist[m_currentPlayIndex];
    if (song.id.isEmpty()) return;
    
    if (checked) {
        m_favoritePage->addSong(song);
    } else {
        m_favoritePage->removeSong(song.id);
    }
}

void MainWindow::updateLikeButtonState(const SongItem &song)
{
    if (m_favoritePage) {
        bool isLiked = m_favoritePage->isFavorite(song.id);
        m_footerWidget->setLiked(isLiked);
    }
}

void MainWindow::onPlaylistClicked()
{
    if (m_playlistWidget->isVisible()) {
        m_playlistWidget->hide();
    } else {
        // Show above the playlist button
        QPoint globalPos = m_footerWidget->mapToGlobal(QPoint(m_footerWidget->width() - 170, -450));
        m_playlistWidget->move(globalPos);
        m_playlistWidget->show();
        m_playlistWidget->raise();
    }
}

void MainWindow::onPlaylistSongClicked(int index)
{
    // When a song in the playlist widget is clicked
    if (index >= 0 && index < m_playlist.size()) {
        m_currentPlayIndex = index;
        const SongItem &song = m_playlist[index];
        
        if (song.playUrl.isEmpty() && !song.id.isEmpty()) {
             m_currentNetworkSong = song;
             m_apiManager->fetchSongUrl(song.id);
             m_playlistWidget->updateCurrentIndex(index);
             m_footerWidget->setSongInfo(song.name, song.artist, song.album, song.coverImage);
        } else {
             playNetworkSong(song, song.playUrl);
        }
    }
}

void MainWindow::onPlaylistClearClicked()
{
    m_playlist.clear();
    m_currentPlayIndex = -1;
    m_playlistWidget->setPlaylist(m_playlist, -1);
    m_audioPlayer->stop();
    m_footerWidget->setSongInfo("", "", "", QPixmap());
}

void MainWindow::onPlayAllSongs(const QList<SongItem> &songs)
{
    if (songs.isEmpty()) return;
    
    m_playlist = songs;
    m_currentPlayIndex = 0;
    m_playlistWidget->setPlaylist(m_playlist, m_currentPlayIndex);
    
    const SongItem &song = m_playlist[0];
    if (song.playUrl.isEmpty() && !song.id.isEmpty()) {
        m_currentNetworkSong = song;
        m_apiManager->fetchSongUrl(song.id);
    } else {
        playNetworkSong(song, song.playUrl);
    }
}

void MainWindow::updateBackButtonState()
{
    if (m_navigationHistory.size() > 1) {
        m_headerWidget->setBackEnabled(true);
    } else {
        m_headerWidget->setBackEnabled(false);
    }
}

void MainWindow::playSongByIndex(int index)
{
    // Not used currently, but kept for compatibility
    Q_UNUSED(index);
}

void MainWindow::onAlbumCoverClicked()
{
    m_menuWidget->hide();
    m_stackedWidget->setCurrentWidget(m_playerDetailPage);
    
    if (m_navigationHistory.isEmpty() || m_navigationHistory.last() != "PlayerDetail") {
        m_navigationHistory.append("PlayerDetail");
    }
    updateBackButtonState();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_videoPlayerWidget && m_videoPlayerWidget->isVisible()) {
        m_videoPlayerWidget->resize(this->size());
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::onVideoClicked(const QString &path)
{
    // Pause music if playing
    if (m_audioPlayer) {
        m_audioPlayer->pause();
    }
    
    if (m_videoPlayerWidget) {
        m_videoPlayerWidget->resize(this->size());
        m_videoPlayerWidget->show();
        m_videoPlayerWidget->raise();
        m_videoPlayerWidget->play(path);
    }
}

void MainWindow::onVideoCloseRequested()
{
    if (m_videoPlayerWidget) {
        m_videoPlayerWidget->stop();
        m_videoPlayerWidget->hide();
    }
}
