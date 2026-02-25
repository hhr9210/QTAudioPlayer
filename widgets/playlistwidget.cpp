#include "playlistwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

PlaylistWidget::PlaylistWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 500);
    
    setupUI();
}

PlaylistWidget::~PlaylistWidget() = default;

void PlaylistWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QFrame *frame = new QFrame();
    frame->setStyleSheet("QFrame { background: white; border-radius: 8px; border: 1px solid #eee; }");
    
    // 阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 50));
    shadow->setOffset(0, 4);
    frame->setGraphicsEffect(shadow);
    
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);
    
    // 标题栏
    QWidget *headerWidget = new QWidget();
    headerWidget->setFixedHeight(50);
    headerWidget->setStyleSheet("border-bottom: 1px solid #e1e1e1;");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    
    m_titleLabel = new QLabel("当前播放");
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; border: none; color: #333333;");
    
    m_countLabel = new QLabel("总 0 首");
    m_countLabel->setStyleSheet("color: #999999; font-size: 12px; border: none;");
    
    m_clearBtn = new QPushButton("清空");
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    m_clearBtn->setStyleSheet("QPushButton { color: #999999; border: none; } QPushButton:hover { color: #333333; }");
    connect(m_clearBtn, &QPushButton::clicked, this, &PlaylistWidget::clearClicked);
    
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addWidget(m_countLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_clearBtn);
    
    frameLayout->addWidget(headerWidget);
    
    // 列表
    m_listWidget = new QListWidget();
    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setStyleSheet(
        "QListWidget::item { height: 40px; padding-left: 10px; }"
        "QListWidget::item:hover { background: #f5f5f5; }"
        "QListWidget::item:selected { background: #fafafa; color: #ec4141; }"
    );
    connect(m_listWidget, &QListWidget::clicked, this, [this](const QModelIndex &index) {
        emit songClicked(index.row());
    });
    
    frameLayout->addWidget(m_listWidget);
    
    mainLayout->addWidget(frame);
}

void PlaylistWidget::setPlaylist(const QList<SongItem> &songs, int currentIndex)
{
    m_listWidget->clear();
    m_countLabel->setText(QString("总 %1 首").arg(songs.size()));
    
    for (int i = 0; i < songs.size(); ++i) {
        const SongItem &song = songs[i];
        QString text = song.name;
        if (!song.artist.isEmpty()) {
            text += " - " + song.artist;
        }
        
        QListWidgetItem *item = new QListWidgetItem(text);
        if (i == currentIndex) {
            item->setText("🎵 " + text);
            item->setSelected(true);
        }
        m_listWidget->addItem(item);
    }
    
    if (currentIndex >= 0 && currentIndex < m_listWidget->count()) {
        m_listWidget->scrollToItem(m_listWidget->item(currentIndex));
    }
}

void PlaylistWidget::updateCurrentIndex(int index)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem *item = m_listWidget->item(i);
        QString text = item->text();
        if (text.startsWith("🎵 ")) {
            item->setText(text.mid(2)); // Remove prefix
        }
        item->setSelected(false);
        
        if (i == index) {
            item->setText("🎵 " + item->text());
            item->setSelected(true);
            m_listWidget->scrollToItem(item);
        }
    }
}

void PlaylistWidget::clear()
{
    m_listWidget->clear();
    m_countLabel->setText("总 0 首");
}
