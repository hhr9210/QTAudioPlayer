#ifndef DISCOVERYPAGE_H
#define DISCOVERYPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QListWidget>
#include <QGridLayout>

class MusicApiManager;
struct SongItem;

class DiscoveryPage : public QWidget
{
    Q_OBJECT

public:
    explicit DiscoveryPage(QWidget *parent = nullptr);
    ~DiscoveryPage() override;

signals:
    void songClicked(const SongItem &song);
    void networkSongDoubleClicked(const SongItem &song);

private slots:
    void updateBanner();
    void onRecommendSongsReceived(const QList<SongItem> &songs);
    void onHotSongsReceived(const QList<SongItem> &songs);
    void onErrorOccurred(const QString &error);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupUI();
    void loadMusicData();
    void createSongCard(const SongItem &song, QGridLayout *layout, int row, int col);
    
    QLabel *m_bannerLabel;
    QTimer *m_bannerTimer;
    int m_currentBannerIndex;
    
    MusicApiManager *m_apiManager;
    QWidget *m_recommendContainer;
    QWidget *m_hotContainer;
    QWidget *m_contentWidget;
    QList<SongItem> m_hotSongs;  // 存储热门歌曲列表
    QList<SongItem> m_recommendSongs;  // 存储推荐歌曲列表
};

#endif // DISCOVERYPAGE_H
