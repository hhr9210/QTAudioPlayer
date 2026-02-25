#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.5);

    // 转发信号
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::playbackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &AudioPlayer::mediaStatusChanged);
}

AudioPlayer::~AudioPlayer() = default;

void AudioPlayer::play()
{
    m_player->play();
}

void AudioPlayer::pause()
{
    m_player->pause();
}

void AudioPlayer::stop()
{
    m_player->stop();
}

void AudioPlayer::setSource(const QUrl &source)
{
    m_player->setSource(source);
}

void AudioPlayer::setPosition(qint64 position)
{
    m_player->setPosition(position);
}

void AudioPlayer::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

QMediaPlayer::PlaybackState AudioPlayer::playbackState() const
{
    return m_player->playbackState();
}

qint64 AudioPlayer::position() const
{
    return m_player->position();
}

qint64 AudioPlayer::duration() const
{
    return m_player->duration();
}

float AudioPlayer::volume() const
{
    return m_audioOutput->volume();
}

