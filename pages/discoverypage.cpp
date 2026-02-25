#include "discoverypage.h"
#include "../core/musicapimanager.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QEvent>
#include <QMouseEvent>

DiscoveryPage::DiscoveryPage(QWidget *parent)
    : QWidget(parent)
    , m_currentBannerIndex(0)
{
    m_apiManager = new MusicApiManager(this);
    
    connect(m_apiManager, &MusicApiManager::recommendSongsReceived,
            this, &DiscoveryPage::onRecommendSongsReceived);
    connect(m_apiManager, &MusicApiManager::hotSongsReceived,
            this, &DiscoveryPage::onHotSongsReceived);
    connect(m_apiManager, &MusicApiManager::errorOccurred,
            this, &DiscoveryPage::onErrorOccurred);
    
    setupUI();
    loadMusicData();
}

DiscoveryPage::~DiscoveryPage() = default;

void DiscoveryPage::setupUI()
{
    QScrollArea *area = new QScrollArea(this);
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setFrameShape(QFrame::NoFrame);
    area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_contentWidget = new QWidget();
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_contentWidget->setAttribute(Qt::WA_StyledBackground);
    m_contentWidget->setStyleSheet("background-color: #ffffff;");

    QVBoxLayout *layout = new QVBoxLayout(m_contentWidget);
    layout->setContentsMargins(30, 20, 30, 20);
    layout->setSpacing(25);

    // Banner轮播
    m_bannerLabel = new QLabel();
    m_bannerLabel->setFixedHeight(200);
    m_bannerLabel->setAlignment(Qt::AlignCenter);
    m_bannerLabel->setStyleSheet("border-radius:10px;font-size:24px;color:white;");
    layout->addWidget(m_bannerLabel);

    m_bannerTimer = new QTimer(this);
    connect(m_bannerTimer, &QTimer::timeout, this, &DiscoveryPage::updateBanner);
    m_bannerTimer->start(3000);
    updateBanner();

    // 推荐歌曲标题
    QLabel *recommendTitle = new QLabel("🎵 推荐歌曲");
    recommendTitle->setStyleSheet(
        "QLabel {"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: #333333;"
        "   padding: 10px 0;"
        "}"
    );
    layout->addWidget(recommendTitle);

    // 推荐歌曲列表
    m_recommendContainer = new QWidget();
    QGridLayout *recLayout = new QGridLayout(m_recommendContainer);
    recLayout->setContentsMargins(0, 0, 0, 0);
    recLayout->setSpacing(15);
    layout->addWidget(m_recommendContainer);

    // 热门歌曲标题
    QLabel *hotTitle = new QLabel("🔥 热门歌曲");
    hotTitle->setStyleSheet(
        "QLabel {"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: #333333;"
        "   padding: 10px 0;"
        "}"
    );
    layout->addWidget(hotTitle);

    // 热门歌曲列表
    m_hotContainer = new QWidget();
    QVBoxLayout *hotLayout = new QVBoxLayout(m_hotContainer);
    hotLayout->setContentsMargins(0, 0, 0, 0);
    hotLayout->setSpacing(0);
    layout->addWidget(m_hotContainer);

    layout->addStretch();

    area->setWidget(m_contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(area);
}

void DiscoveryPage::loadMusicData()
{
    // 加载推荐歌曲
    m_apiManager->fetchRecommendSongs(12);
    
    // 加载热门歌曲
    m_apiManager->fetchHotSongs(10);
}

void DiscoveryPage::updateBanner()
{
    QStringList colors = {
        "#ff9a9e", "#a1c4fd", "#84fab0"
    };
    m_bannerLabel->setStyleSheet(
        "background:" + colors[m_currentBannerIndex] +
        ";border-radius:10px;color:white;font-size:24px;"
    );
    m_bannerLabel->setText(QString("推荐 %1").arg(m_currentBannerIndex + 1));
    m_currentBannerIndex = (m_currentBannerIndex + 1) % colors.size();
}

void DiscoveryPage::onRecommendSongsReceived(const QList<SongItem> &songs)
{
    QGridLayout *layout = qobject_cast<QGridLayout*>(m_recommendContainer->layout());
    if (!layout) return;

    // 清除旧条目
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    m_recommendSongs = songs;
    int columns = 5; // Reduced from 6 to fit width better without scrolling

    for (int i = 0; i < songs.size(); ++i) {
        const SongItem &song = songs[i];
        
        QWidget *cardWidget = new QWidget();
        cardWidget->setMinimumWidth(130);
        cardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        cardWidget->setProperty("songIndex", i);
        cardWidget->setProperty("isRecommend", true);
        cardWidget->installEventFilter(this);
        cardWidget->setCursor(Qt::PointingHandCursor);
        
        QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(5, 5, 5, 5);
        cardLayout->setSpacing(8);
        
        // 封面
        QLabel *coverLabel = new QLabel();
        coverLabel->setFixedSize(130, 130);
        coverLabel->setAlignment(Qt::AlignCenter);
        
        if (!song.coverImage.isNull()) {
             coverLabel->setPixmap(song.coverImage.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));
             coverLabel->setStyleSheet("border-radius: 8px; background: transparent;");
        } else {
            coverLabel->setStyleSheet(
                "QLabel {"
                "   background-color: #f5f5f5;"
                "   border-radius: 8px;"
                "   color: #999999;"
                "   font-size: 40px;"
                "}"
            );
            coverLabel->setText("🎵");
        }
        
        // 居中放置封面
        QHBoxLayout *coverLayout = new QHBoxLayout();
        coverLayout->addStretch();
        coverLayout->addWidget(coverLabel);
        coverLayout->addStretch();
        cardLayout->addLayout(coverLayout);
        
        // 歌曲名
        QLabel *nameLabel = new QLabel(song.name);
        nameLabel->setStyleSheet(
            "QLabel {"
            "   font-size: 13px;"
            "   font-weight: bold;"
            "   color: #333333;"
            "}"
        );
        nameLabel->setWordWrap(false); // 单行
        nameLabel->setText(song.name.length() > 10 ? song.name.left(9) + "..." : song.name);
        nameLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(nameLabel);
        
        // 歌手
        QLabel *artistLabel = new QLabel(song.artist);
        artistLabel->setStyleSheet(
            "QLabel {"
            "   font-size: 11px;"
            "   color: #666666;"
            "}"
        );
        artistLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(artistLabel);

        layout->addWidget(cardWidget, i / columns, i % columns);
    }
}

void DiscoveryPage::onHotSongsReceived(const QList<SongItem> &songs)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_hotContainer->layout());
    if (!layout) return;

    // 清除旧条目
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    
    m_hotSongs = songs;
    
    for (int i = 0; i < songs.size(); ++i) {
        const SongItem &song = songs[i];
        
        QWidget *itemWidget = new QWidget();
        itemWidget->setFixedHeight(60);
        itemWidget->setStyleSheet("QWidget:hover { background: #f5f5f5; border-radius: 4px; }");
        itemWidget->setProperty("songIndex", i);
        itemWidget->setProperty("isRecommend", false);
        itemWidget->installEventFilter(this);
        itemWidget->setCursor(Qt::PointingHandCursor);
        
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 5, 10, 5);
        itemLayout->setSpacing(15);
        
        // 排名
        QLabel *rankLabel = new QLabel(QString::number(i + 1));
        rankLabel->setFixedWidth(30);
        rankLabel->setAlignment(Qt::AlignCenter);
        rankLabel->setStyleSheet(
            QString("QLabel {"
                    "   font-size: 16px;"
                    "   font-weight: bold;"
                    "   color: %1;"
                    "   background: transparent;"
                    "}").arg(i < 3 ? "#ec4141" : "#999")
        );
        itemLayout->addWidget(rankLabel);
        
        // 歌曲信息
        QVBoxLayout *infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(4);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        
        QLabel *nameLabel = new QLabel(song.name);
        nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333; background: transparent;");
        infoLayout->addWidget(nameLabel);
        
        QString artistInfo = song.artist;
        if (!song.album.isEmpty()) {
            artistInfo += " · " + song.album;
        }
        QLabel *artistLabel = new QLabel(artistInfo);
        artistLabel->setStyleSheet("font-size: 12px; color: #666; background: transparent;");
        infoLayout->addWidget(artistLabel);
        
        itemLayout->addLayout(infoLayout, 1);
        itemLayout->addStretch();
        
        layout->addWidget(itemWidget);
        
        // 分割线
        if (i < songs.size() - 1) {
            QFrame *line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Plain);
            line->setStyleSheet("color: #eee;");
            layout->addWidget(line);
        }
    }
}

void DiscoveryPage::onErrorOccurred(const QString &error)
{
    qDebug() << "API Error:" << error;
    // 可以显示错误提示
}

bool DiscoveryPage::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *widget = qobject_cast<QWidget*>(watched);
        if (widget) {
            QVariant indexVar = widget->property("songIndex");
            QVariant isRecommendVar = widget->property("isRecommend");
            
            if (indexVar.isValid() && isRecommendVar.isValid()) {
                int index = indexVar.toInt();
                bool isRecommend = isRecommendVar.toBool();
                
                if (isRecommend && index >= 0 && index < m_recommendSongs.size()) {
                    emit networkSongDoubleClicked(m_recommendSongs[index]);
                    return true;
                } else if (!isRecommend && index >= 0 && index < m_hotSongs.size()) {
                    emit networkSongDoubleClicked(m_hotSongs[index]);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
