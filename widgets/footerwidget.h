#ifndef FOOTERWIDGET_H
#define FOOTERWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

class AudioPlayer;
class ScrollingLabel;


//底部播放烂
class FooterWidget : public QFrame
{
    Q_OBJECT

public:
    explicit FooterWidget(AudioPlayer *audioPlayer, QWidget *parent = nullptr);
    ~FooterWidget() override;

    void setSongInfo(const QString &title, const QString &artist, const QString &album, const QPixmap &cover);
    void setSongName(const QString &name); // 保持兼容性
    void setPlaybackState(bool isPlaying);
    void setLiked(bool isLiked);

signals:
    void playClicked();
    void pauseClicked();
    void prevClicked();
    void nextClicked();
    void albumCoverClicked();
    void likeClicked(bool checked);
    void playlistClicked();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onPlaybackStateChanged();

private:
    AudioPlayer *m_audioPlayer;
    QLabel *m_coverLabel;
    ScrollingLabel *m_songNameLabel;
    ScrollingLabel *m_artistLabel;
    QPushButton *m_likeBtn;     // 喜欢按钮
    QPushButton *m_playBtn;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;
    QPushButton *m_playlistBtn; // 播放列表按钮
    QSlider *m_progressSlider;
    QSlider *m_volumeSlider;    // 音量滑块
    QLabel *m_timeLabel;        // 总时长
    QLabel *m_currentTimeLabel; // 当前时长
    bool m_isDraggingSlider;
};

#endif // FOOTERWIDGET_H
