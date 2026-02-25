#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QObject>

class AudioPlayer : public QObject
{
    Q_OBJECT  //预处理宏，负责反射，反射

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer() override;

    // 播放控制
    void play();
    void pause();
    void stop();
    void setSource(const QUrl &source);
    void setPosition(qint64 position);
    void setVolume(float volume);

    // 状态查询
    QMediaPlayer::PlaybackState playbackState() const;
    qint64 position() const;
    qint64 duration() const;
    float volume() const;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};

#endif // AUDIOPLAYER_H

