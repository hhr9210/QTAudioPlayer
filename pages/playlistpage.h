#ifndef PLAYLISTPAGE_H
#define PLAYLISTPAGE_H

#include <QWidget>

class PlaylistPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistPage(QWidget *parent = nullptr);
    ~PlaylistPage() override;
};

#endif // PLAYLISTPAGE_H

