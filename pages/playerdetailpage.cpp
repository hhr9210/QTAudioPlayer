#include "playerdetailpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>

PlayerDetailPage::PlayerDetailPage(QWidget *parent)
    : QWidget(parent)
    , m_vinylWidget(nullptr)
    , m_lyricsWidget(nullptr)
{
    setupUI();
}

PlayerDetailPage::~PlayerDetailPage()
{
}

void PlayerDetailPage::setupUI()
{
    // Main layout for the page (contains the scroll area)
    QVBoxLayout *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);

    // --- Close Button Bar (Overlay or Top) ---
    // We put it above the scroll area
    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background: transparent;");
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(20, 0, 20, 0);
    
    QPushButton *closeBtn = new QPushButton("﹀", topBar);
    closeBtn->setFixedSize(40, 40);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { "
        "   color: #666666; "
        "   font-size: 24px; "
        "   font-weight: bold; "
        "   border: none; "
        "   background: transparent; "
        "} "
        "QPushButton:hover { color: #333333; }"
    );
    connect(closeBtn, &QPushButton::clicked, this, &PlayerDetailPage::backClicked);
    
    topBarLayout->addWidget(closeBtn);
    topBarLayout->addStretch(); // Push button to left (or addStretch before to push to right)
    
    pageLayout->addWidget(topBar);

    // Scroll Area
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    // Make scroll area transparent so we can see the background drawn in paintEvent
    scrollArea->setStyleSheet("QScrollArea { background: transparent; } QWidget#ScrollContent { background: transparent; }");
    // Hide scrollbars for cleaner look (optional, but requested implicitly by 'immersive')
    // scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 

    // Scroll Content Widget
    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("ScrollContent");
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(40, 0, 40, 40); // Top margin 0 because we have topBar
    contentLayout->setSpacing(40);

    // --- Top Section: Vinyl + Lyrics ---
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(40);

    // 1. Vinyl Widget (Left)
    m_vinylWidget = new VinylWidget(scrollContent);
    m_vinylWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topLayout->addWidget(m_vinylWidget, 1);

    // 2. Lyrics and Info (Right)
    QWidget *infoContainer = new QWidget();
    infoContainer->setStyleSheet("background-color: rgba(0, 0, 0, 0.03); border-radius: 15px;");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(20, 20, 20, 20);

    m_titleLabel = new QLabel("Song Title");
    m_titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #333333; background: transparent;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget(m_titleLabel);

    m_artistLabel = new QLabel("Artist Name");
    m_artistLabel->setStyleSheet("font-size: 18px; color: #666666; background: transparent;");
    m_artistLabel->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget(m_artistLabel);
    
    infoLayout->addSpacing(20);

    // Inner scroll area for lyrics (so lyrics can scroll independently if needed, or just be part of the page)
    // If the user wants "scroll down for comments", usually the whole page scrolls. 
    // But lyrics often have their own scroll. Let's make lyrics fixed height or expanded, 
    // but here let's assume standard layout: Vinyl + Lyrics block.
    // For now, I'll keep lyrics as a label inside.
    
    m_lyricsWidget = new LyricsWidget(this);
    m_lyricsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoLayout->addWidget(m_lyricsWidget, 1);

    topLayout->addWidget(infoContainer, 1);
    
    contentLayout->addLayout(topLayout);

    // --- Bottom Section: Comments ---
    QWidget *commentsContainer = new QWidget();
    commentsContainer->setStyleSheet("background-color: rgba(0, 0, 0, 0.03); border-radius: 15px;");
    QVBoxLayout *commentsLayout = new QVBoxLayout(commentsContainer);
    commentsLayout->setContentsMargins(20, 20, 20, 20);
    commentsLayout->setSpacing(15);

    QLabel *commentsHeader = new QLabel("Comments (Hot)");
    commentsHeader->setStyleSheet("font-size: 20px; font-weight: bold; color: #333333; background: transparent;");
    commentsLayout->addWidget(commentsHeader);

    // Add dummy comments
    for (int i = 1; i <= 5; ++i) {
        QWidget *commentItem = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(commentItem);
        
        QLabel *avatar = new QLabel();
        avatar->setFixedSize(40, 40);
        avatar->setStyleSheet("background-color: #e1e1e1; border-radius: 20px;");
        itemLayout->addWidget(avatar);

        QVBoxLayout *textLayout = new QVBoxLayout();
        QLabel *user = new QLabel(QString("User %1").arg(i));
        user->setStyleSheet("color: #666666; font-weight: bold;");
        QLabel *content = new QLabel("This is a wonderful song! I really like the vibe.");
        content->setStyleSheet("color: #333333;");
        content->setWordWrap(true);
        
        textLayout->addWidget(user);
        textLayout->addWidget(content);
        itemLayout->addLayout(textLayout);
        
        commentsLayout->addWidget(commentItem);
        
        // Divider
        if (i < 5) {
            QFrame *line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setStyleSheet("color: #e1e1e1;");
            commentsLayout->addWidget(line);
        }
    }

    contentLayout->addWidget(commentsContainer);
    contentLayout->addStretch(); // Push everything up

    scrollArea->setWidget(scrollContent);
    pageLayout->addWidget(scrollArea);
}

void PlayerDetailPage::setSongInfo(const QString &title, const QString &artist, const QPixmap &cover)
{
    m_titleLabel->setText(title);
    m_artistLabel->setText(artist);
    if (m_vinylWidget) {
        m_vinylWidget->setCover(cover);
    }
}

void PlayerDetailPage::setPlaybackState(bool isPlaying)
{
    if (m_vinylWidget) {
        m_vinylWidget->setPlaybackState(isPlaying);
    }
}

void PlayerDetailPage::loadLyrics(const QString &lrcPath)
{
    if (m_lyricsWidget) {
        if (lrcPath.isEmpty()) {
            m_lyricsWidget->clear();
        } else {
            QList<LrcLine> lyrics = LrcParser::parseFile(lrcPath);
            m_lyricsWidget->setLyrics(lyrics);
        }
    }
}

void PlayerDetailPage::updatePosition(qint64 position)
{
    if (m_lyricsWidget) {
        m_lyricsWidget->setCurrentTime(position);
    }
}

void PlayerDetailPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background (Light Theme Gradient)
    QLinearGradient bgGradient(0, 0, width(), height());
    bgGradient.setColorAt(0, QColor(255, 255, 255));
    bgGradient.setColorAt(1, QColor(245, 245, 245));
    painter.fillRect(rect(), bgGradient);
}
