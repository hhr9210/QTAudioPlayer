#include "scrollinglabel.h"
#include <QPainter>
#include <QFontMetrics>
#include <QResizeEvent>

ScrollingLabel::ScrollingLabel(QWidget *parent)
    : QLabel(parent)
    , m_scrollPosition(0)
    , m_needsScrolling(false)
{
    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setInterval(30); // 30ms 更新一次，流畅滚动
    connect(m_scrollTimer, &QTimer::timeout, this, &ScrollingLabel::updateScroll);
}

ScrollingLabel::~ScrollingLabel() = default;

void ScrollingLabel::setText(const QString &text)
{
    m_fullText = text;
    m_scrollPosition = 0;
    checkScrolling();
    update();
}

void ScrollingLabel::checkScrolling()
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(m_fullText);
    int labelWidth = width();
    
    m_needsScrolling = (textWidth > labelWidth);
    
    if (m_needsScrolling) {
        if (!m_scrollTimer->isActive()) {
            m_scrollTimer->start();
        }
    } else {
        m_scrollTimer->stop();
        m_scrollPosition = 0;
    }
}

void ScrollingLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(m_fullText);
    
    if (!m_needsScrolling || textWidth <= width()) {
        // 不需要滚动，正常显示（左对齐）
        painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_fullText);
    } else {
        // 需要滚动，绘制滚动文本
        int x = -m_scrollPosition;
        int y = (height() + fm.ascent() - fm.descent()) / 2;
        
        // 绘制文本（可能需要绘制两次以实现循环效果）
        painter.drawText(x, y, m_fullText);
        
        // 如果滚动到末尾，从左侧继续绘制（循环效果）
        if (m_scrollPosition + width() >= textWidth) {
            int secondX = x + textWidth + 50; // 50px 间距
            painter.drawText(secondX, y, m_fullText);
        }
    }
}

void ScrollingLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    checkScrolling();
}

void ScrollingLabel::updateScroll()
{
    if (!m_needsScrolling) {
        return;
    }
    
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(m_fullText);
    
    m_scrollPosition += 1; // 每次移动1像素
    
    // 如果滚动到末尾，重置位置（实现循环滚动）
    if (m_scrollPosition >= textWidth + 50) {
        m_scrollPosition = 0;
    }
    
    update();
}

