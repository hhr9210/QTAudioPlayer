#include "menuwidget.h"

MenuWidget::MenuWidget(QWidget *parent)
    : QListWidget(parent)
{
    setFixedWidth(200);

    QStringList menuItems = {
        "发现音乐",
        "本地音乐",
        "MV",
        "我喜欢的音乐",
        "创建的歌单",
        "每日推荐",
        "排行榜"
    };

    addItems(menuItems);
    setStyleSheet(
        "QListWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-right: 1px solid #e1e1e1;"
        "   outline: none;"
        "   padding-top: 10px;"
        "}"
        "QListWidget::item {"
        "   height: 36px;"
        "   margin: 2px 12px;"
        "   border-radius: 4px;"
        "   padding-left: 10px;"
        "   color: #333333;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #f5f5f7;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #e6e6e6;"
        "   color: #000000;"
        "   font-weight: bold;"
        "   border-left: 3px solid #ec4141;"
        "}"
    );
}

MenuWidget::~MenuWidget() = default;

