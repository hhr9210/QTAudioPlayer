#include "searchresultpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QFrame>
#include <QEvent>
#include <QMouseEvent>

SearchResultPage::SearchResultPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *area = new QScrollArea(this);
    area->setWidgetResizable(true);
    area->setFrameShape(QFrame::NoFrame);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background: white;");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(30, 20, 30, 20);
    contentLayout->setSpacing(20);

    m_titleLabel = new QLabel("搜索结果");
    m_titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #333;");
    contentLayout->addWidget(m_titleLabel);

    m_resultsContainer = new QWidget();
    QVBoxLayout *resultsLayout = new QVBoxLayout(m_resultsContainer);
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->setSpacing(0);
    contentLayout->addWidget(m_resultsContainer);

    contentLayout->addStretch();
    area->setWidget(contentWidget);
    mainLayout->addWidget(area);
}

SearchResultPage::~SearchResultPage() = default;

void SearchResultPage::setSearchResults(const QList<SongItem> &songs)
{
    m_songs = songs;
    m_titleLabel->setText(QString("搜索结果 (%1首)").arg(songs.size()));

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_resultsContainer->layout());
    if (!layout) return;

    // Clear old results
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (int i = 0; i < songs.size(); ++i) {
        const SongItem &song = songs[i];
        
        QWidget *itemWidget = new QWidget();
        itemWidget->setFixedHeight(60);
        itemWidget->setStyleSheet("QWidget:hover { background: #f5f5f5; border-radius: 4px; }");
        itemWidget->setProperty("songIndex", i);
        itemWidget->installEventFilter(this);
        itemWidget->setCursor(Qt::PointingHandCursor);
        
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 5, 10, 5);
        itemLayout->setSpacing(15);
        
        // Number
        QLabel *numLabel = new QLabel(QString::number(i + 1).rightJustified(2, '0'));
        numLabel->setFixedWidth(30);
        numLabel->setAlignment(Qt::AlignCenter);
        numLabel->setStyleSheet("color: #999999; font-size: 14px; background: transparent;");
        itemLayout->addWidget(numLabel);
        
        // Info
        QVBoxLayout *infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(4);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        
        QLabel *nameLabel = new QLabel(song.name);
        nameLabel->setStyleSheet("font-size: 14px; color: #333333; background: transparent;");
        infoLayout->addWidget(nameLabel);
        
        QString artistInfo = song.artist;
        if (!song.album.isEmpty()) {
            artistInfo += " - " + song.album;
        }
        QLabel *artistLabel = new QLabel(artistInfo);
        artistLabel->setStyleSheet("font-size: 12px; color: #666666; background: transparent;");
        infoLayout->addWidget(artistLabel);
        
        itemLayout->addLayout(infoLayout);
        
        itemLayout->addStretch();
        
        layout->addWidget(itemWidget);
        
        // Divider
        if (i < songs.size() - 1) {
            QFrame *line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Plain);
            line->setStyleSheet("color: #e1e1e1;");
            layout->addWidget(line);
        }
    }
}

bool SearchResultPage::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *widget = qobject_cast<QWidget*>(watched);
        if (widget) {
            QVariant indexVar = widget->property("songIndex");
            if (indexVar.isValid()) {
                int index = indexVar.toInt();
                if (index >= 0 && index < m_songs.size()) {
                    emit songDoubleClicked(m_songs[index]);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
