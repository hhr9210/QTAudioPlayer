#include "vinylwidget.h"
#include <QPainter>
#include <QPainterPath>

VinylWidget::VinylWidget(QWidget *parent)
    : QWidget(parent)
    , m_rotationAngle(0.0)
    , m_isPlaying(false)
{
    // Set a minimum size to ensure it's visible
    setMinimumSize(300, 300);

    m_rotationTimer = new QTimer(this);
    m_rotationTimer->setInterval(16); // ~60 FPS
    connect(m_rotationTimer, &QTimer::timeout, this, [this]() {
        m_rotationAngle += 0.5; // Rotate speed
        if (m_rotationAngle >= 360.0) m_rotationAngle -= 360.0;
        update(); // Trigger repaint
    });
}

VinylWidget::~VinylWidget()
{
}

void VinylWidget::setCover(const QPixmap &cover)
{
    m_coverPixmap = cover;
    update();
}

void VinylWidget::setPlaybackState(bool isPlaying)
{
    m_isPlaying = isPlaying;
    if (m_isPlaying) {
        m_rotationTimer->start();
    } else {
        m_rotationTimer->stop();
    }
    update();
}

void VinylWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Use floating point coordinates for center to avoid jitter
    qreal w = width();
    qreal h = height();
    QPointF center(w / 2.0, h / 2.0);
    
    // Determine size based on the smaller dimension
    qreal diameter = qMin(w, h) - 40.0;
    if (diameter < 100.0) diameter = 100.0;
    qreal radius = diameter / 2.0;

    painter.translate(center);
    painter.rotate(m_rotationAngle);

    // 1. Vinyl Outer Edge (Black)
    painter.setBrush(QColor(10, 10, 10));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(0, 0), radius, radius);

    // 2. Vinyl Grooves (Textures)
    painter.setBrush(Qt::NoBrush);
    QPen groovePen(QColor(30, 30, 30));
    groovePen.setWidthF(1.0);
    painter.setPen(groovePen);
    for (qreal r = radius - 5.0; r > radius * 0.4; r -= 4.0) {
        painter.drawEllipse(QPointF(0, 0), r, r);
    }

    // 3. Album Cover
    qreal coverRadius = radius * 0.65;
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), coverRadius, coverRadius);
    painter.setClipPath(path);

    if (!m_coverPixmap.isNull()) {
        // Draw pixmap centered
        QRectF targetRect(-coverRadius, -coverRadius, coverRadius * 2, coverRadius * 2);
        painter.drawPixmap(targetRect, m_coverPixmap, m_coverPixmap.rect());
    } else {
        painter.setBrush(QColor(50, 50, 50));
        painter.setPen(Qt::NoPen);
        painter.drawRect(QRectF(-coverRadius, -coverRadius, coverRadius * 2, coverRadius * 2));
        
        // Placeholder Icon
        painter.setPen(QColor(200, 200, 200));
        QFont f = painter.font();
        f.setPixelSize(static_cast<int>(coverRadius));
        painter.setFont(f);
        painter.drawText(QRectF(-coverRadius, -coverRadius, coverRadius * 2, coverRadius * 2), Qt::AlignCenter, "🎵");
    }
}
