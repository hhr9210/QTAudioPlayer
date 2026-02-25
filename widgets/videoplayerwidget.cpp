#include "videoplayerwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QShowEvent>
#include <QTimer>

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_videoWidget = new QVideoWidget(this);
    m_isSeeking = false;

    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(m_videoWidget);
    m_audioOutput->setVolume(1.0);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_videoWidget);

    m_overlayWindow = new QWidget(nullptr);
    m_overlayWindow->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_overlayWindow->setAttribute(Qt::WA_TranslucentBackground);
    m_overlayWindow->setAttribute(Qt::WA_ShowWithoutActivating);
    m_overlayWindow->setFocusPolicy(Qt::NoFocus);

    m_controlsWidget = new QWidget(m_overlayWindow);
    m_controlsWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0.8); border-radius: 10px;");

    auto *controlsLayout = new QHBoxLayout(m_controlsWidget);
    controlsLayout->setContentsMargins(20, 10, 20, 10);

    m_playBtn = new QPushButton("⏸", m_controlsWidget);
    m_playBtn->setFixedSize(40, 40);
    m_playBtn->setStyleSheet("color: white; font-size: 20px; border: none; background: transparent;");
    connect(m_playBtn, &QPushButton::clicked, this, [this]() {
        if (m_player->playbackState() == QMediaPlayer::PlayingState) {
            m_player->pause();
            m_playBtn->setText("▶");
        } else {
            m_player->play();
            m_playBtn->setText("⏸");
        }
        updateControls();
    });

    m_timeLabel = new QLabel("00:00 / 00:00", m_controlsWidget);
    m_timeLabel->setStyleSheet("color: white; background: transparent; border: none;");

    m_slider = new QSlider(Qt::Horizontal, m_controlsWidget);
    m_slider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #666; border-radius: 2px; }"
        "QSlider::handle:horizontal { background: white; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }"
        "QSlider::sub-page:horizontal { background: #ec4141; border-radius: 2px; }"
    );
    connect(m_slider, &QSlider::sliderMoved, m_player, &QMediaPlayer::setPosition);
    connect(m_slider, &QSlider::sliderPressed, this, [this]() {
        m_isSeeking = true;
        updateControls();
    });
    connect(m_slider, &QSlider::sliderReleased, this, [this]() {
        m_player->setPosition(m_slider->value());
        m_isSeeking = false;
        updateControls();
    });
    connect(m_slider, &QSlider::actionTriggered, this, [this](int action) {
        if (action == QAbstractSlider::SliderMove ||
            action == QAbstractSlider::SliderSingleStepAdd ||
            action == QAbstractSlider::SliderSingleStepSub ||
            action == QAbstractSlider::SliderPageStepAdd ||
            action == QAbstractSlider::SliderPageStepSub ||
            action == QAbstractSlider::SliderToMinimum ||
            action == QAbstractSlider::SliderToMaximum) {
            m_isSeeking = true;
            m_player->setPosition(m_slider->value());
            m_isSeeking = false;
            updateControls();
        }
    });

    controlsLayout->addWidget(m_playBtn);
    controlsLayout->addWidget(m_timeLabel);
    controlsLayout->addWidget(m_slider);

    m_closeBtn = new QPushButton("✕", m_overlayWindow);
    m_closeBtn->setFixedSize(40, 40);
    m_closeBtn->setFocusPolicy(Qt::NoFocus);
    m_closeBtn->setStyleSheet(
        "QPushButton { color: white; background: rgba(0,0,0,0.5); border-radius: 20px; font-size: 20px; border: 2px solid white; }"
        "QPushButton:hover { background: rgba(255,0,0,0.8); border-color: red; }"
    );
    connect(m_closeBtn, &QPushButton::clicked, this, [this]() {
        stop();
        emit closeRequested();
    });

    connect(m_player, &QMediaPlayer::positionChanged, this, &VideoPlayerWidget::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &VideoPlayerWidget::onDurationChanged);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setInterval(2500);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, [this]() {
        if (m_player->playbackState() == QMediaPlayer::PlayingState) {
            m_overlayWindow->hide();
            setCursor(Qt::BlankCursor);
        }
    });

    setMouseTracking(true);
    m_videoWidget->setMouseTracking(true);
    m_videoWidget->installEventFilter(this);
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    if (m_overlayWindow) {
        m_overlayWindow->hide();
        delete m_overlayWindow;
        m_overlayWindow = nullptr;
    }
}

void VideoPlayerWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (window()) {
        window()->installEventFilter(this);
    }
}

bool VideoPlayerWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_videoWidget) {
        if (event->type() == QEvent::MouseMove) {
            updateControls();
        } else if (event->type() == QEvent::MouseButtonPress) {
            if (m_player->playbackState() == QMediaPlayer::PlayingState) {
                m_player->pause();
                m_playBtn->setText("▶");
            } else {
                m_player->play();
                m_playBtn->setText("⏸");
            }
            updateControls();
        }
    } else if (watched == window()) {
        if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
            updateOverlayGeometry();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void VideoPlayerWidget::play(const QString &path)
{
    m_player->setSource(QUrl::fromLocalFile(path));
    m_player->play();
    m_playBtn->setText("⏸");
    show();
    raise();
    updateControls();
}

void VideoPlayerWidget::stop()
{
    m_player->stop();
    m_player->setSource(QUrl());
    if (m_overlayWindow) {
        m_overlayWindow->hide();
    }
    hide();
    setCursor(Qt::ArrowCursor);
}

void VideoPlayerWidget::onPositionChanged(qint64 position)
{
    if (m_isSeeking) {
        return;
    }
    if (!m_slider->isSliderDown()) {
        m_slider->setValue(position);
    }
    QString current = formatTime(position);
    QString total = formatTime(m_player->duration());
    m_timeLabel->setText(current + " / " + total);
}

void VideoPlayerWidget::onDurationChanged(qint64 duration)
{
    m_slider->setRange(0, duration);
    QString current = formatTime(m_player->position());
    QString total = formatTime(duration);
    m_timeLabel->setText(current + " / " + total);
}

void VideoPlayerWidget::updateControls()
{
    if (!m_overlayWindow) {
        return;
    }

    updateOverlayGeometry();
    m_overlayWindow->show();
    m_overlayWindow->raise();
    setCursor(Qt::ArrowCursor);
    m_hideTimer->start();
}

void VideoPlayerWidget::updateOverlayGeometry()
{
    if (!m_overlayWindow || !isVisible()) {
        return;
    }

    QPoint globalTopLeft = mapToGlobal(QPoint(0, 0));
    QSize s = size();
    m_overlayWindow->setGeometry(QRect(globalTopLeft, s));

    int margin = 20;
    int closeX = s.width() - m_closeBtn->width() - margin;
    int closeY = margin;
    m_closeBtn->move(closeX, closeY);

    int controlsHeight = 60;
    int maxW = 800;
    int w = s.width() - 2 * margin;
    if (w > maxW) {
        w = maxW;
    }
    int x = (s.width() - w) / 2;
    int y = s.height() - controlsHeight - 40;
    m_controlsWidget->setGeometry(x, y, w, controlsHeight);
}

QString VideoPlayerWidget::formatTime(qint64 ms)
{
    int seconds = (ms / 1000) % 60;
    int minutes = (ms / 60000);
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void VideoPlayerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateOverlayGeometry();
}

void VideoPlayerWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    updateControls();
}
