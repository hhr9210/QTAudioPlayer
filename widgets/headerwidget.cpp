#include "headerwidget.h"
#include <QHBoxLayout>
#include <QLabel>

HeaderWidget::HeaderWidget(QWidget *parent)
    : QFrame(parent)
{
    setFixedHeight(54);
    setStyleSheet(
        "background:#ec4141;"
        "border-top-left-radius:8px;"
        "border-top-right-radius:8px;"
        "border-bottom:none;"
    );

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 0, 15, 0);
    layout->setSpacing(10);

    QLabel *logoText = new QLabel("网易云音乐");
    logoText->setStyleSheet(
        "color:white;"
        "font-size:22px;"
        "font-weight:bold;"
        "padding-left:5px;"
        "font-family:'Microsoft YaHei', sans-serif;"
    );
    layout->addWidget(logoText);
    layout->addSpacing(10);

    m_backBtn = new QPushButton("◀");
    m_forwardBtn = new QPushButton("▶");
    for (auto btn : {m_backBtn, m_forwardBtn}) {
        btn->setFixedSize(28, 28);
        btn->setStyleSheet(
            "QPushButton{border:none;background:rgba(0,0,0,0.1);color:white;font-size:16px;border-radius:14px;}"
            "QPushButton:hover{background:rgba(0,0,0,0.2);}"
        );
    }
    layout->addWidget(m_backBtn);
    layout->addWidget(m_forwardBtn);
    m_forwardBtn->setVisible(false);

    connect(m_backBtn, &QPushButton::clicked, this, &HeaderWidget::backClicked);

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("搜索音乐 / 歌手 / 专辑");
    m_searchEdit->setFixedHeight(30);
    m_searchEdit->setStyleSheet(
        "QLineEdit{"
        "background:rgba(0,0,0,0.1);"
        "border:none;"
        "border-radius:15px;"
        "padding-left:12px;"
        "color:white;"
        "}"
        "QLineEdit::placeholder{color:rgba(255,255,255,0.6);}"
    );
    layout->addWidget(m_searchEdit, 1);

    connect(m_searchEdit, &QLineEdit::textChanged, this, &HeaderWidget::searchTextChanged);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, [this]() {
        emit searchRequested(m_searchEdit->text());
    });

    QLabel *vipLabel = new QLabel("VIP");
    vipLabel->setStyleSheet(
        "background:#ffd666;color:#333;"
        "border-radius:4px;"
        "padding:2px 6px;"
        "font-size:12px;"
    );
    layout->addWidget(vipLabel);

    QLabel *avatar = new QLabel();
    avatar->setFixedSize(32, 32);
    avatar->setStyleSheet(
        "background:#535353;"
        "border-radius:16px;"
    );
    layout->addWidget(avatar);

    QLabel *userName = new QLabel("User");
    userName->setStyleSheet("color:white;font-size:13px;");
    layout->addWidget(userName);

    QLabel *arrow = new QLabel("▼");
    arrow->setStyleSheet("color:white;font-size:10px;");
    layout->addWidget(arrow);

    layout->addSpacing(15);

    QPushButton *minBtn = new QPushButton("—");
    QPushButton *maxBtn = new QPushButton("□");
    QPushButton *closeBtn = new QPushButton("✕");

    QString winBtnStyle =
        "QPushButton{"
        "color:white;"
        "border:none;"
        "width:36px;"
        "height:28px;"
        "}"
        "QPushButton:hover{background:rgba(255,255,255,0.2);}";

    minBtn->setStyleSheet(winBtnStyle);
    maxBtn->setStyleSheet(winBtnStyle);
    closeBtn->setStyleSheet(
        "QPushButton{color:white;border:none;width:36px;height:28px;}"
        "QPushButton:hover{background:#c72c2c;}"
    );

    layout->addWidget(minBtn);
    layout->addWidget(maxBtn);
    layout->addWidget(closeBtn);

    QWidget *parentWindow = parentWidget();
    if (parentWindow) {
        connect(minBtn, &QPushButton::clicked, parentWindow, &QWidget::showMinimized);
        connect(maxBtn, &QPushButton::clicked, parentWindow, [parentWindow]() {
            if (parentWindow->isMaximized()) {
                parentWindow->showNormal();
            } else {
                parentWindow->showMaximized();
            }
        });
        connect(closeBtn, &QPushButton::clicked, parentWindow, &QWidget::close);
    }
}

HeaderWidget::~HeaderWidget() = default;

void HeaderWidget::setBackEnabled(bool enabled)
{
    m_backBtn->setEnabled(enabled);
    if (enabled) {
        m_backBtn->setStyleSheet(
            "QPushButton{border:none;background:rgba(0,0,0,0.1);color:white;font-size:16px;border-radius:14px;}"
            "QPushButton:hover{background:rgba(0,0,0,0.2);}"
        );
    } else {
        m_backBtn->setStyleSheet(
            "QPushButton{border:none;background:rgba(0,0,0,0.05);color:rgba(255,255,255,0.4);font-size:16px;border-radius:14px;}"
        );
    }
}

