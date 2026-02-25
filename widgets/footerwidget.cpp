#include "footerwidget.h"
#include "../core/audioplayer.h"
#include "scrollinglabel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QEvent>
#include <QMouseEvent>
#include <QTime>

FooterWidget::FooterWidget(AudioPlayer *audioPlayer, QWidget *parent)
    : QFrame(parent)
    , m_audioPlayer(audioPlayer)
    , m_isDraggingSlider(false)
{
    setFixedHeight(72);
    setStyleSheet(
        "FooterWidget { background: #ffffff; border-top: 1px solid #e1e1e1; }"
        "QToolTip { color: #333; background: #fff; border: 1px solid #ccc; }"
    );

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 0, 10, 0);
    mainLayout->setSpacing(0);

    // ===========================
    // Left: Song Info
    // ===========================
    QWidget *leftWidget = new QWidget();
    leftWidget->setFixedWidth(250);
    QHBoxLayout *leftLayout = new QHBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 5, 0, 5);
    leftLayout->setSpacing(10);

    // Cover
    m_coverLabel = new QLabel();
    m_coverLabel->setFixedSize(48, 48);
    m_coverLabel->setAlignment(Qt::AlignCenter);
    m_coverLabel->setText("🎵");
    m_coverLabel->setStyleSheet(
        "QLabel {"
        "   background: #f0f0f0;"
        "   border-radius: 4px;"
        "   border: 1px solid #e1e1e1;"
        "   font-size: 24px;"
        "   color: #999999;"
        "}"
    );
    m_coverLabel->installEventFilter(this);
    m_coverLabel->setCursor(Qt::PointingHandCursor);
    leftLayout->addWidget(m_coverLabel);

    // Info (Title + Artist)
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);
    infoLayout->setAlignment(Qt::AlignVCenter);

    m_songNameLabel = new ScrollingLabel();
    m_songNameLabel->setText("未在播放");
    m_songNameLabel->setStyleSheet("color: #333333; font-size: 14px;");
    m_songNameLabel->setFixedHeight(20);
    
    m_artistLabel = new ScrollingLabel();
    m_artistLabel->setText("");
    m_artistLabel->setStyleSheet("color: #666666; font-size: 12px;");
    m_artistLabel->setFixedHeight(16);
    
    infoLayout->addWidget(m_songNameLabel);
    infoLayout->addWidget(m_artistLabel);
    leftLayout->addLayout(infoLayout);

    // Like Button
    m_likeBtn = new QPushButton("🤍");
    m_likeBtn->setFixedSize(24, 24);
    m_likeBtn->setCheckable(true);
    m_likeBtn->setCursor(Qt::PointingHandCursor);
    m_likeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #666666; font-size: 16px; border: none; }"
        "QPushButton:checked { color: #ec4141; }"
        "QPushButton:hover { color: #333333; }"
    );
    connect(m_likeBtn, &QPushButton::clicked, this, [this](bool checked) {
        setLiked(checked);
        emit likeClicked(checked);
    });
    leftLayout->addWidget(m_likeBtn);

    mainLayout->addWidget(leftWidget);

    // ===========================
    // Center: Controls
    // ===========================
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 5, 0, 5);
    centerLayout->setSpacing(0);
    centerLayout->setAlignment(Qt::AlignCenter);

    // Control Buttons Row
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);
    btnLayout->setAlignment(Qt::AlignCenter);

    // Prev
    m_prevBtn = new QPushButton("⏮");
    m_prevBtn->setFixedSize(28, 28);
    m_prevBtn->setCursor(Qt::PointingHandCursor);
    m_prevBtn->setStyleSheet("QPushButton { background: transparent; border: none; color: #ec4141; font-size: 16px; } QPushButton:hover { color: #d73535; }");
    btnLayout->addWidget(m_prevBtn);

    // Play/Pause
    m_playBtn = new QPushButton("▶");
    m_playBtn->setFixedSize(36, 36);
    m_playBtn->setCursor(Qt::PointingHandCursor);
    m_playBtn->setStyleSheet(
        "QPushButton {"
        "   background: #ec4141; border-radius: 18px; color: #ffffff; font-size: 18px; border: none;"
        "}"
        "QPushButton:hover { background: #d73535; }"
    );
    btnLayout->addWidget(m_playBtn);

    // Next
    m_nextBtn = new QPushButton("⏭");
    m_nextBtn->setFixedSize(28, 28);
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    m_nextBtn->setStyleSheet("QPushButton { background: transparent; border: none; color: #ec4141; font-size: 16px; } QPushButton:hover { color: #d73535; }");
    btnLayout->addWidget(m_nextBtn);

    centerLayout->addLayout(btnLayout);

    // Progress Bar Row
    QHBoxLayout *progressLayout = new QHBoxLayout();
    progressLayout->setSpacing(10);
    
    // Time Labels
    m_currentTimeLabel = new QLabel("00:00");
    m_currentTimeLabel->setStyleSheet("color: #333333; font-size: 11px; min-width: 40px;");
    m_currentTimeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    m_timeLabel = new QLabel("00:00"); // Total time
    m_timeLabel->setStyleSheet("color: #333333; font-size: 11px; min-width: 40px;");
    m_timeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Progress Slider
    m_progressSlider = new QSlider(Qt::Horizontal);
    m_progressSlider->setRange(0, 100);
    m_progressSlider->setFixedHeight(15);
    m_progressSlider->setCursor(Qt::PointingHandCursor);
    m_progressSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #e1e1e1; border-radius: 2px; }"
        "QSlider::sub-page:horizontal { background: #ec4141; border-radius: 2px; }"
        "QSlider::sub-page:horizontal:hover { background: #d73535; }"
        "QSlider::handle:horizontal { background: #ec4141; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; border: 2px solid #fff; }"
        "QSlider::handle:horizontal:hover { transform: scale(1.2); }"
    );

    progressLayout->addWidget(m_currentTimeLabel);
    progressLayout->addWidget(m_progressSlider);
    progressLayout->addWidget(m_timeLabel);

    centerLayout->addLayout(progressLayout);
    mainLayout->addWidget(centerWidget, 1);

    // ===========================
    // Right: Volume & List
    // ===========================
    QWidget *rightWidget = new QWidget();
    rightWidget->setFixedWidth(200);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    rightLayout->setAlignment(Qt::AlignRight);

    // Volume Icon (Simple Label or Btn)
    QLabel *volIcon = new QLabel("🔊");
    volIcon->setStyleSheet("color: #666666; font-size: 16px;");
    rightLayout->addWidget(volIcon);

    // Volume Slider
    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(100); // Default
    m_volumeSlider->setFixedWidth(80);
    m_volumeSlider->setCursor(Qt::PointingHandCursor);
    m_volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #e1e1e1; border-radius: 2px; }"
        "QSlider::sub-page:horizontal { background: #ec4141; border-radius: 2px; }"
        "QSlider::sub-page:horizontal:hover { background: #d73535; }"
        "QSlider::handle:horizontal { background: #ffffff; width: 10px; height: 10px; margin: -3px 0; border-radius: 5px; border: 1px solid #ccc; }"
    );
    rightLayout->addWidget(m_volumeSlider);

    // Playlist Btn
    m_playlistBtn = new QPushButton("📜");
    m_playlistBtn->setFixedSize(24, 24);
    m_playlistBtn->setCursor(Qt::PointingHandCursor);
    m_playlistBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #666666; font-size: 18px; border: none; }"
        "QPushButton:hover { color: #333333; }"
    );
    connect(m_playlistBtn, &QPushButton::clicked, this, &FooterWidget::playlistClicked);
    rightLayout->addWidget(m_playlistBtn);

    mainLayout->addWidget(rightWidget);

    // Connections
    connect(m_prevBtn, &QPushButton::clicked, this, &FooterWidget::prevClicked);
    connect(m_nextBtn, &QPushButton::clicked, this, &FooterWidget::nextClicked);
    connect(m_playBtn, &QPushButton::clicked, this, [this]() {
        if (m_audioPlayer->playbackState() == QMediaPlayer::PlayingState) {
            emit pauseClicked();
        } else {
            emit playClicked();
        }
    });

    // Slider Logic
    connect(m_progressSlider, &QSlider::sliderPressed, this, [this]() { m_isDraggingSlider = true; });
    connect(m_progressSlider, &QSlider::sliderReleased, this, [this]() {
        m_isDraggingSlider = false;
        if (m_audioPlayer->duration() > 0) {
            m_audioPlayer->setPosition(m_progressSlider->value() * m_audioPlayer->duration() / 100);
        }
    });
    connect(m_progressSlider, &QSlider::valueChanged, this, [this](int value) {
        if (m_isDraggingSlider && m_audioPlayer->duration() > 0) {
            qint64 pos = value * m_audioPlayer->duration() / 100;
            QTime t((pos / 3600000), (pos / 60000) % 60, (pos / 1000) % 60);
            QString format = (m_audioPlayer->duration() > 3600000) ? "hh:mm:ss" : "mm:ss";
            m_currentTimeLabel->setText(t.toString(format));
        }
    });

    // Volume Logic
    connect(m_volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        if (m_audioPlayer) m_audioPlayer->setVolume(value / 100.0f);
    });

    // Audio Player Signals
    if (m_audioPlayer) {
        connect(m_audioPlayer, &AudioPlayer::positionChanged, this, &FooterWidget::onPositionChanged);
        connect(m_audioPlayer, &AudioPlayer::durationChanged, this, &FooterWidget::onDurationChanged);
        connect(m_audioPlayer, &AudioPlayer::playbackStateChanged, this, &FooterWidget::onPlaybackStateChanged);
        
        // Sync volume
        m_volumeSlider->setValue(m_audioPlayer->volume() * 100);
    }
}

FooterWidget::~FooterWidget() = default;

void FooterWidget::setSongInfo(const QString &title, const QString &artist, const QString &album, const QPixmap &cover)
{
    QString titleText = title.isEmpty() ? "未知歌曲" : title;
    m_songNameLabel->setText(titleText);
    
    QString artistText = artist.isEmpty() ? "" : artist;
    if (!album.isEmpty()) {
        artistText += artistText.isEmpty() ? album : " · " + album;
    }
    m_artistLabel->setText(artistText);
    
    if (!cover.isNull()) {
        m_coverLabel->setPixmap(cover.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_coverLabel->setText("");
    } else {
        m_coverLabel->setText("🎵");
        m_coverLabel->setPixmap(QPixmap());
    }
}

void FooterWidget::setLiked(bool isLiked)
{
    m_likeBtn->setChecked(isLiked);
    m_likeBtn->setText(isLiked ? "❤️" : "🤍");
}

void FooterWidget::setPlaybackState(bool isPlaying)
{
    if (isPlaying) {
        m_playBtn->setText("⏸");
    } else {
        m_playBtn->setText("▶");
    }
}

void FooterWidget::setSongName(const QString &name)
{
    m_songNameLabel->setText(name);
    m_artistLabel->setText("");
}

void FooterWidget::onPositionChanged(qint64 position)
{
    if (!m_isDraggingSlider && m_audioPlayer->duration() > 0) {
        m_progressSlider->setValue(position * 100 / m_audioPlayer->duration());
    }
    
    qint64 duration = m_audioPlayer->duration();
    QTime currentTime((position / 3600000), (position / 60000) % 60, (position / 1000) % 60);
    
    QString format = "mm:ss";
    if (duration > 3600000) format = "hh:mm:ss";
    
    m_currentTimeLabel->setText(currentTime.toString(format));
}

void FooterWidget::onDurationChanged(qint64 duration)
{
    QTime totalTime((duration / 3600000), (duration / 60000) % 60, (duration / 1000) % 60);
    
    QString format = "mm:ss";
    if (duration > 3600000) format = "hh:mm:ss";
    
    m_timeLabel->setText(totalTime.toString(format));
}

void FooterWidget::onPlaybackStateChanged()
{
    if (m_audioPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_playBtn->setText("⏸");
    } else {
        m_playBtn->setText("▶");
    }
}

bool FooterWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_coverLabel && event->type() == QEvent::MouseButtonPress) {
        emit albumCoverClicked();
        return true;
    }
    return QFrame::eventFilter(watched, event);
}
