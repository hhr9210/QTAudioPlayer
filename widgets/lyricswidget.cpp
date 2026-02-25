#include "lyricswidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QStyleOption>

LyricsWidget::LyricsWidget(QWidget *parent)
    : QWidget(parent)
{
    m_normalFont = QFont("Microsoft YaHei", 12);
    m_highlightFont = QFont("Microsoft YaHei", 16, QFont::Bold);
    
    // Enable mouse tracking for hover effects if needed (not needed for simple lyrics)
}

void LyricsWidget::setLyrics(const QList<LrcLine> &lyrics)
{
    m_lyrics = lyrics;
    m_currentIndex = -1;
    m_scrollOffset = 0;
    m_targetScrollOffset = 0;
    update();
}

void LyricsWidget::clear()
{
    m_lyrics.clear();
    m_currentIndex = -1;
    update();
}

void LyricsWidget::setCurrentTime(qint64 time)
{
    if (m_lyrics.isEmpty()) return;
    
    m_currentTime = time;
    int index = getIndexForTime(time);
    
    if (index != m_currentIndex) {
        m_currentIndex = index;
        
        // Calculate target offset to center the current line
        // Center of widget: height() / 2
        // Position of current line: index * m_lineHeight
        // Offset needed: (index * m_lineHeight) - (height() / 2) + (m_lineHeight / 2)
        
        if (m_currentIndex >= 0) {
            m_targetScrollOffset = (m_currentIndex * m_lineHeight) - (height() / 2) + (m_lineHeight / 2);
        } else {
            m_targetScrollOffset = 0;
        }
        
        update();
    }
}

int LyricsWidget::getIndexForTime(qint64 time)
{
    if (m_lyrics.isEmpty()) return -1;
    
    // Find the last line that has time <= current time
    for (int i = m_lyrics.size() - 1; i >= 0; --i) {
        if (m_lyrics[i].time <= time) {
            return i;
        }
    }
    return 0; // Default to first line if before start
}

void LyricsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_lyrics.isEmpty()) {
        painter.setPen(m_normalColor);
        painter.setFont(m_normalFont);
        painter.drawText(rect(), Qt::AlignCenter, "暂无歌词");
        return;
    }
    
    // Smooth scrolling interpolation (simple linear approach for now, or just jump)
    // For smoother experience, we could use a timer or QPropertyAnimation, 
    // but for simplicity, let's just use a simple interpolation or direct set for now.
    // Let's implement a simple "approach" logic if we were using a timer loop, 
    // but since we are driven by setCurrentTime which is called frequently (e.g. every 100ms or less),
    // we can just interpolate m_scrollOffset towards m_targetScrollOffset.
    
    double diff = m_targetScrollOffset - m_scrollOffset;
    if (qAbs(diff) > 1.0) {
        m_scrollOffset += diff * 0.1; // Ease factor
        update(); // Trigger another paint
    } else {
        m_scrollOffset = m_targetScrollOffset;
    }
    
    int centerY = height() / 2;
    
    for (int i = 0; i < m_lyrics.size(); ++i) {
        int y = (i * m_lineHeight) - m_scrollOffset + centerY - (m_lineHeight/2);
        
        // Optimization: Don't draw if out of screen
        if (y < -m_lineHeight || y > height()) continue;
        
        if (i == m_currentIndex) {
            painter.setPen(m_highlightColor);
            painter.setFont(m_highlightFont);
        } else {
            painter.setPen(m_normalColor);
            painter.setFont(m_normalFont);
        }
        
        QRect lineRect(20, y, width() - 40, m_lineHeight);
        painter.drawText(lineRect, Qt::AlignCenter, m_lyrics[i].text);
    }
}

void LyricsWidget::wheelEvent(QWheelEvent *event)
{
    // Allow manual scrolling
    m_targetScrollOffset -= event->angleDelta().y();
    update();
}
