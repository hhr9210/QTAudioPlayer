#include "localmusicpage.h"
#include "../core/localmusicmanager.h"
#include "../core/songinfo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>

LocalMusicPage::LocalMusicPage(LocalMusicManager *musicManager, QWidget *parent)
    : QWidget(parent)
    , m_musicManager(musicManager)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(15);

    // 标题栏
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("🎵 本地音乐");
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "   color: #ffffff;"
        "}"
    );
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    
    QPushButton *refreshBtn = new QPushButton("🔄 刷新列表");
    refreshBtn->setStyleSheet(
        "QPushButton {"
        "   background: #1DB954;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 16px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: #1ed760;"
        "}"
        "QPushButton:pressed {"
        "   background: #1aa34a;"
        "}"
    );
    connect(refreshBtn, &QPushButton::clicked, this, &LocalMusicPage::refreshMusicList);
    titleLayout->addWidget(refreshBtn);
    layout->addLayout(titleLayout);

    setupTable();
    layout->addWidget(m_musicTableWidget);

    if (m_musicManager) {
        connect(m_musicManager, &LocalMusicManager::musicListRefreshed,
                this, &LocalMusicPage::populateTable);
    }

    refreshMusicList();
}

LocalMusicPage::~LocalMusicPage() = default;

void LocalMusicPage::setupTable()
{
    m_musicTableWidget = new QTableWidget(this);
    m_musicTableWidget->setColumnCount(5);
    m_musicTableWidget->setHorizontalHeaderLabels({"封面", "歌曲名", "歌手", "专辑", "时长"});
    
    m_musicTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_musicTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_musicTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_musicTableWidget->setAlternatingRowColors(true);
    m_musicTableWidget->setShowGrid(false);
    
    // 设置列宽
    m_musicTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 封面
    m_musicTableWidget->setColumnWidth(0, 70);
    
    m_musicTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 歌曲名
    m_musicTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // 歌手
    m_musicTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // 专辑
    
    m_musicTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed); // 时长
    m_musicTableWidget->setColumnWidth(4, 80);
    
    m_musicTableWidget->horizontalHeader()->setStretchLastSection(false);
    m_musicTableWidget->verticalHeader()->setVisible(false);
    m_musicTableWidget->setRowHeight(0, 70); // 设置行高
    
    m_musicTableWidget->setStyleSheet(
        "QTableWidget {"
        "   border: none;"
        "   border-radius: 8px;"
        "   background: #ffffff;"
        "   gridline-color: transparent;"
        "   selection-background-color: #f0f0f0;"
        "   selection-color: #ec4141;"
        "}"
        "QTableWidget::item {"
        "   padding: 10px 8px;"
        "   border: none;"
        "   color: #333333;"
        "   border-bottom: 1px solid #f5f5f5;"
        "}"
        "QTableWidget::item:hover {"
        "   background: #f9f9f9;"
        "}"
        "QTableWidget::item:selected {"
        "   background: #f0f0f0;"
        "   color: #ec4141;"
        "}"
        "QHeaderView::section {"
        "   background: #ffffff;"
        "   padding: 12px 8px;"
        "   border: none;"
        "   border-bottom: 1px solid #e1e1e1;"
        "   color: #666666;"
        "   font-weight: 600;"
        "   font-size: 13px;"
        "}"
        "QTableWidget::item:first {"
        "   border-top-left-radius: 8px;"
        "   border-top-right-radius: 8px;"
        "}"
    );
    
    connect(m_musicTableWidget, &QTableWidget::cellDoubleClicked,
            this, [this](int row, int) {
                emit songDoubleClicked(row);
            });
}

void LocalMusicPage::populateTable()
{
    if (!m_musicManager) return;
    
    m_musicTableWidget->setRowCount(0);
    
    for (int i = 0; i < m_musicManager->songCount(); ++i) {
        SongInfo info = m_musicManager->getSongInfo(i);
        
        int row = m_musicTableWidget->rowCount();
        m_musicTableWidget->insertRow(row);
        m_musicTableWidget->setRowHeight(row, 70);
        
        // 封面
        QLabel *coverLabel = new QLabel();
        coverLabel->setAlignment(Qt::AlignCenter);
        if (!info.cover.isNull()) {
            coverLabel->setPixmap(info.cover.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            coverLabel->setText("🎵");
            coverLabel->setStyleSheet(
                "QLabel {"
                "   background-color: #f5f5f5;"
                "   border-radius: 6px;"
                "   font-size: 28px;"
                "   color: #999999;"
                "}"
            );
            coverLabel->setFixedSize(60, 60);
        }
        m_musicTableWidget->setCellWidget(row, 0, coverLabel);
        
        // 歌曲名（支持滚动）
        QTableWidgetItem *titleItem = new QTableWidgetItem(info.title);
        titleItem->setToolTip(info.title); // 鼠标悬停显示完整文本
        m_musicTableWidget->setItem(row, 1, titleItem);
        
        // 歌手（支持滚动）
        QString artistText = info.artist.isEmpty() ? "未知歌手" : info.artist;
        QTableWidgetItem *artistItem = new QTableWidgetItem(artistText);
        artistItem->setToolTip(artistText);
        m_musicTableWidget->setItem(row, 2, artistItem);
        
        // 专辑（支持滚动）
        QString albumText = info.album.isEmpty() ? "未知专辑" : info.album;
        QTableWidgetItem *albumItem = new QTableWidgetItem(albumText);
        albumItem->setToolTip(albumText);
        m_musicTableWidget->setItem(row, 3, albumItem);
        
        // 时长
        QTableWidgetItem *durationItem = new QTableWidgetItem(info.formattedDuration());
        durationItem->setTextAlignment(Qt::AlignCenter);
        durationItem->setForeground(QBrush(QColor("#666666")));
        m_musicTableWidget->setItem(row, 4, durationItem);
    }
}

void LocalMusicPage::refreshMusicList()
{
    if (m_musicManager) {
        m_musicManager->refreshMusicList();
    }
}
