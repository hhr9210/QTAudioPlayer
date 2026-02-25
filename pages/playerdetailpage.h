#ifndef PLAYERDETAILPAGE_H
#define PLAYERDETAILPAGE_H

#include <QWidget>
#include <QLabel>
#include "../widgets/vinylwidget.h"
#include "../widgets/lyricswidget.h"

class PlayerDetailPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerDetailPage(QWidget *parent = nullptr);
    ~PlayerDetailPage();

    void setSongInfo(const QString &title, const QString &artist, const QPixmap &cover);
    void setPlaybackState(bool isPlaying);
    
    // Lyrics support
    void loadLyrics(const QString &lrcPath);
    void updatePosition(qint64 position);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();

    QLabel *m_titleLabel;
    QLabel *m_artistLabel;
    LyricsWidget *m_lyricsWidget;
    
    VinylWidget *m_vinylWidget;

signals:
    void backClicked();
};

#endif // PLAYERDETAILPAGE_H
