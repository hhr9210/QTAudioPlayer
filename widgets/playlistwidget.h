#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include "../core/musicapimanager.h" // for SongItem

class PlaylistWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistWidget(QWidget *parent = nullptr);
    ~PlaylistWidget() override;

    void setPlaylist(const QList<SongItem> &songs, int currentIndex);
    void updateCurrentIndex(int index);
    void clear();

signals:
    void songClicked(int index);
    void clearClicked();

private:
    QListWidget *m_listWidget;
    QLabel *m_titleLabel;
    QLabel *m_countLabel;
    QPushButton *m_clearBtn;
    
    void setupUI();
};

#endif // PLAYLISTWIDGET_H
