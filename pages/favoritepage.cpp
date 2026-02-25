#include "favoritepage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

FavoritePage::FavoritePage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 20, 30, 20);
    layout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel("❤️");
    icon->setStyleSheet("font-size: 40px; color: #ec4141;");
    headerLayout->addWidget(icon);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *title = new QLabel("我喜欢的音乐");
    title->setStyleSheet("font-size:24px;font-weight:bold;color:#333333;");
    
    QLabel *info = new QLabel("User · 2024-01-01 创建");
    info->setStyleSheet("font-size:12px;color:#666666;");
    
    titleLayout->addWidget(title);
    titleLayout->addWidget(info);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    
    layout->addLayout(headerLayout);

    // Action Bar
    QHBoxLayout *actionLayout = new QHBoxLayout();
    QPushButton *playAllBtn = new QPushButton("播放全部");
    playAllBtn->setCursor(Qt::PointingHandCursor);
    playAllBtn->setStyleSheet(
        "QPushButton { background: #ec4141; color: white; border-radius: 16px; padding: 6px 15px; font-size: 14px; }"
        "QPushButton:hover { background: #ff5555; }"
    );
    connect(playAllBtn, &QPushButton::clicked, this, [this]() {
        if (!m_songs.isEmpty()) {
            emit playAllRequested(m_songs);
        }
    });
    
    actionLayout->addWidget(playAllBtn);
    actionLayout->addStretch();
    layout->addLayout(actionLayout);

    // Song Table
    m_table = new QTableWidget();
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"歌曲标题", "歌手", "专辑"});
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setFocusPolicy(Qt::NoFocus);
    
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        if (row >= 0 && row < m_songs.size()) {
            emit songDoubleClicked(m_songs[row]);
        }
    });
    
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    m_table->setStyleSheet(
        "QTableWidget { background: transparent; border: none; alternate-background-color: #fafafa; }"
        "QTableWidget::item { padding: 8px; border: none; color: #333333; }"
        "QTableWidget::item:selected { background: #e6e6e6; color: #ec4141; }"
        "QHeaderView::section { background: white; border: none; padding: 8px; font-weight: normal; color: #666666; }"
    );

    layout->addWidget(m_table);
    
    // Initial empty state
    refreshTable();
}

FavoritePage::~FavoritePage() = default;

void FavoritePage::addSong(const SongItem &song)
{
    if (isFavorite(song.id)) return;
    
    m_songs.append(song);
    refreshTable();
}

void FavoritePage::removeSong(const QString &songId)
{
    for (int i = 0; i < m_songs.size(); ++i) {
        if (m_songs[i].id == songId) {
            m_songs.removeAt(i);
            refreshTable();
            return;
        }
    }
}

bool FavoritePage::isFavorite(const QString &songId) const
{
    for (const SongItem &song : m_songs) {
        if (song.id == songId) return true;
    }
    return false;
}

void FavoritePage::refreshTable()
{
    m_table->setRowCount(m_songs.size());
    for(int i=0; i<m_songs.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(m_songs[i].name));
        m_table->setItem(i, 1, new QTableWidgetItem(m_songs[i].artist));
        m_table->setItem(i, 2, new QTableWidgetItem(m_songs[i].album));
        
        m_table->item(i, 1)->setForeground(QBrush(QColor("#b3b3b3")));
        m_table->item(i, 2)->setForeground(QBrush(QColor("#b3b3b3")));
    }
}
