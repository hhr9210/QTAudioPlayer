#ifndef LYRICSWIDGET_H
#define LYRICSWIDGET_H

#include <QWidget>
#include <QList>
#include "../core/lrcparser.h"

class LyricsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LyricsWidget(QWidget *parent = nullptr);
    
    void setLyrics(const QList<LrcLine> &lyrics);
    void setCurrentTime(qint64 time);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QList<LrcLine> m_lyrics;
    int m_currentIndex = -1;
    qint64 m_currentTime = 0;
    
    // Formatting
    int m_lineHeight = 40;
    QFont m_normalFont;
    QFont m_highlightFont;
    QColor m_normalColor = QColor(200, 200, 200);
    QColor m_highlightColor = QColor(255, 255, 255);
    
    // Scrolling
    double m_scrollOffset = 0;
    double m_targetScrollOffset = 0;
    
    int getIndexForTime(qint64 time);
};

#endif // LYRICSWIDGET_H
