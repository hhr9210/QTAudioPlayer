#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QPushButton>
#include <QLabel>
#include <QSlider>

class QTimer;
class QResizeEvent;
class QMouseEvent;
class QShowEvent;

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

    void play(const QString &path);
    void stop();

signals:
    void closeRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void updateControls();

private:
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QAudioOutput *m_audioOutput;

    QWidget *m_overlayWindow;

    QWidget *m_controlsWidget;
    QPushButton *m_playBtn;
    QSlider *m_slider;
    QLabel *m_timeLabel;
    QPushButton *m_closeBtn;

    bool m_isSeeking;

    void updateOverlayGeometry();
    QString formatTime(qint64 ms);
    QTimer *m_hideTimer;
};

#endif // VIDEOPLAYERWIDGET_H
