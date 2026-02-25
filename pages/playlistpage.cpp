#include "playlistpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>

PlaylistPage::PlaylistPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("📁 我的歌单");
    title->setStyleSheet("font-size:24px;font-weight:bold;color:#333;");
    layout->addWidget(title);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);

    QStringList playlists = {
        "开车必备",
        "学习专注",
        "运动健身",
        "深夜emo",
        "旅行在路上",
        "经典老歌",
        "2024年度歌单"
    };

    for (int i = 0; i < playlists.size(); ++i) {
        QFrame *playlistFrame = new QFrame();
        playlistFrame->setFixedSize(150, 180);
        playlistFrame->setStyleSheet(
            "QFrame {"
            "   background: #ffffff;"
            "   border: 1px solid #e1e1e1;"
            "   border-radius: 10px;"
            "}"
            "QFrame:hover {"
            "   background: #f5f5f5;"
            "   border: 1px solid #cccccc;"
            "}"
        );

        QVBoxLayout *playlistLayout = new QVBoxLayout(playlistFrame);
        playlistLayout->setContentsMargins(10, 10, 10, 10);
        playlistLayout->setSpacing(10);

        QLabel *cover = new QLabel("🎵");
        cover->setAlignment(Qt::AlignCenter);
        cover->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff5f6d, stop:1 #ffc371);"
            "border-radius: 8px;"
            "color: white;"
            "font-size: 40px;"
        );
        cover->setFixedHeight(100);

        QLabel *playlistName = new QLabel(playlists[i]);
        playlistName->setStyleSheet(
            "font-size:14px;"
            "font-weight:bold;"
            "color:#333;"
        );
        playlistName->setWordWrap(true);
        playlistName->setAlignment(Qt::AlignCenter);

        playlistLayout->addWidget(cover);
        playlistLayout->addWidget(playlistName);
        playlistLayout->addStretch();

        gridLayout->addWidget(playlistFrame, i/3, i%3);
    }

    layout->addLayout(gridLayout);
    layout->addStretch();
}

PlaylistPage::~PlaylistPage() = default;

