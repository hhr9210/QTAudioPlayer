#ifndef VINYLWIDGET_H
#define VINYLWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>

class VinylWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VinylWidget(QWidget *parent = nullptr);
    ~VinylWidget();

    void setCover(const QPixmap &cover);
    void setPlaybackState(bool isPlaying);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_coverPixmap;
    qreal m_rotationAngle;
    QTimer *m_rotationTimer;
    bool m_isPlaying;
};

#endif // VINYLWIDGET_H
