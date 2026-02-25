#include "dailyrecommendpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDate>

DailyRecommendPage::DailyRecommendPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    QLabel *title = new QLabel("✨ 每日推荐");
    title->setStyleSheet("font-size:24px;font-weight:bold;color:#333333;");
    layout->addWidget(title);

    QLabel *date = new QLabel(QDate::currentDate().toString("yyyy年MM月dd日"));
    date->setStyleSheet("font-size:14px;color:#666666;");
    layout->addWidget(date);

    layout->addStretch();
}

DailyRecommendPage::~DailyRecommendPage() = default;

