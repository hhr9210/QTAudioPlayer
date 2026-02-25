#ifndef FAVORITEPAGE_H
#define FAVORITEPAGE_H

#include <QWidget>
#include <QList>
#include "../core/musicapimanager.h" // For SongItem

class QTableWidget;

class FavoritePage : public QWidget
{
    Q_OBJECT

public:
    explicit FavoritePage(QWidget *parent = nullptr);
    ~FavoritePage() override;

    void addSong(const SongItem &song);
    void removeSong(const QString &songId);
    bool isFavorite(const QString &songId) const;

signals:
    void songDoubleClicked(const SongItem &song);
    void playAllRequested(const QList<SongItem> &songs);

private:
    QTableWidget *m_table;
    QList<SongItem> m_songs;
    
    void refreshTable();
};

#endif // FAVORITEPAGE_H

