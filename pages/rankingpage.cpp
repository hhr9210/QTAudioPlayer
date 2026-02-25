#include "rankingpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>

RankingPage::RankingPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("🏆 排行榜");
    title->setStyleSheet("font-size:24px;font-weight:bold;color:#333333;");
    layout->addWidget(title);

    QStringList rankings = {
        "热歌榜",
        "新歌榜",
        "飙升榜",
        "原创榜",
        "电音榜",
        "说唱榜",
        "民谣榜"
    };

    for (int i = 0; i < rankings.size(); ++i) {
        QFrame *rankFrame = new QFrame();
        rankFrame->setStyleSheet(
            "QFrame{"
            "background:#ffffff;"
            "border-radius:8px;"
            "border:1px solid #e1e1e1;"
            "}"
        );
        rankFrame->setFixedHeight(50);

        QHBoxLayout *rankLayout = new QHBoxLayout(rankFrame);
        rankLayout->setContentsMargins(15, 0, 15, 0);

        QLabel *rankNum = new QLabel(QString::number(i+1));
        rankNum->setStyleSheet(
            QString("font-size:16px;font-weight:bold;color:%1;")
                .arg(i < 3 ? "#ec4141" : "#999999")
        );
        rankNum->setFixedWidth(30);

        QLabel *rankName = new QLabel(rankings[i]);
        rankName->setStyleSheet("font-size:14px;color:#333333;");

        rankLayout->addWidget(rankNum);
        rankLayout->addWidget(rankName);
        rankLayout->addStretch();

        layout->addWidget(rankFrame);
    }

    layout->addStretch();
}

RankingPage::~RankingPage() = default;

