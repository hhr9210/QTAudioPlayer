#include "mvpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QMediaPlayer>
#include <QUrl>
#include <QPushButton>

#include <QFileIconProvider>

MVPage::MVPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    m_contentWidget = new QWidget();
    m_contentWidget->setStyleSheet("background: #ffffff;");
    
    scrollArea->setWidget(m_contentWidget);
    mainLayout->addWidget(scrollArea);

    loadLocalVideos();
    setupUI();
}

MVPage::~MVPage() = default;

void MVPage::loadLocalVideos()
{
    m_videos.clear();
    QDir dir("E:/qttest/mp/Videos");
    if (!dir.exists()) {
        qDebug() << "Videos directory does not exist.";
        return;
    }

    QStringList filters = {"*.mp4", "*.mkv", "*.avi", "*.mov"};
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &fileInfo : fileList) {
        VideoItem video;
        video.fileName = fileInfo.completeBaseName();
        video.filePath = fileInfo.absoluteFilePath();
        
        // Try to load a thumbnail image if it exists (same name as video but jpg/png)
        QString baseName = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName();
        if (QFile::exists(baseName + ".jpg")) {
            video.thumbnail = QPixmap(baseName + ".jpg");
        } else if (QFile::exists(baseName + ".png")) {
            video.thumbnail = QPixmap(baseName + ".png");
        } else {
            // Use system icon as fallback
            QFileIconProvider provider;
            QIcon icon = provider.icon(fileInfo);
            if (!icon.isNull()) {
                video.thumbnail = icon.pixmap(320, 180);
            }
        }
        
        // Duration fallback (file size for now)
        double sizeMB = fileInfo.size() / (1024.0 * 1024.0);
        video.durationStr = QString("%1 MB").arg(sizeMB, 0, 'f', 1);

        m_videos.append(video);
    }
}

void MVPage::setupUI()
{
    // Clear existing layout if any (though this is called once in constructor)
    if (m_contentWidget->layout()) {
        delete m_contentWidget->layout();
    }

    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(30, 20, 30, 20);
    contentLayout->setSpacing(20);

    QLabel *title = new QLabel("🎬 MV专区");
    title->setStyleSheet("font-size:24px;font-weight:bold;color:#333333;");
    contentLayout->addWidget(title);

    if (m_videos.isEmpty()) {
        QLabel *emptyLabel = new QLabel("暂无本地视频，请在 Videos 文件夹中添加视频文件");
        emptyLabel->setStyleSheet("color: #666666; font-size: 14px;");
        contentLayout->addWidget(emptyLabel);
        contentLayout->addStretch();
        return;
    }

    // Grid Layout for Videos
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    int columns = 3; // Adjust based on preference or make dynamic
    
    for (int i = 0; i < m_videos.size(); ++i) {
        const VideoItem &video = m_videos[i];
        
        QFrame *cardFrame = new QFrame();
        cardFrame->setFixedSize(220, 200); // Fixed size to prevent layout issues
        cardFrame->setStyleSheet("QFrame { background: transparent; border-radius: 8px; } QFrame:hover { background: #f5f5f5; }");
        
        QVBoxLayout *frameLayout = new QVBoxLayout(cardFrame);
        frameLayout->setContentsMargins(10, 10, 10, 10);
        frameLayout->setSpacing(8);
        
        // Thumbnail
        QLabel *thumbLabel = new QLabel();
        thumbLabel->setFixedSize(200, 112); // Fixed 16:9 size
        thumbLabel->setStyleSheet("background: #000000; border-radius: 6px; color: white; font-size: 24px; border: none;");
        thumbLabel->setAlignment(Qt::AlignCenter);
        thumbLabel->setScaledContents(true);
        
        if (!video.thumbnail.isNull()) {
            thumbLabel->setPixmap(video.thumbnail);
        } else {
            thumbLabel->setText("▶");
        }
        frameLayout->addWidget(thumbLabel);
        
        // Info
        QLabel *nameLabel = new QLabel(video.fileName);
        nameLabel->setStyleSheet("font-size: 14px; color: #333333; background: transparent; border: none; font-weight: bold;");
        nameLabel->setFixedHeight(20);
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        frameLayout->addWidget(nameLabel);
        
        QLabel *durationLabel = new QLabel(video.durationStr);
        durationLabel->setStyleSheet("font-size: 12px; color: #888888; background: transparent; border: none;");
        durationLabel->setFixedHeight(16);
        durationLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        frameLayout->addWidget(durationLabel);
        
        frameLayout->addStretch();
        
        // Clickable overlay button
        QPushButton *overlayBtn = new QPushButton(cardFrame);
        overlayBtn->resize(cardFrame->size());
        overlayBtn->setStyleSheet("background: transparent; border: none;");
        overlayBtn->setCursor(Qt::PointingHandCursor);
        
        connect(overlayBtn, &QPushButton::clicked, this, [this, video](){
            emit videoClicked(video.filePath);
        });
        
        gridLayout->addWidget(cardFrame, i / columns, i % columns);
    }

    contentLayout->addLayout(gridLayout);
    contentLayout->addStretch();
}
