#ifndef LOCALMUSICPAGE_H
#define LOCALMUSICPAGE_H

#include <QWidget>
#include <QTableWidget>

class LocalMusicManager;

class LocalMusicPage : public QWidget
{
    Q_OBJECT

public:
    explicit LocalMusicPage(LocalMusicManager *musicManager, QWidget *parent = nullptr);
    ~LocalMusicPage() override;

    void refreshMusicList();

signals:
    void songDoubleClicked(int index);

private:
    LocalMusicManager *m_musicManager;
    QTableWidget *m_musicTableWidget;
    
    void setupTable();
    void populateTable();
};

#endif // LOCALMUSICPAGE_H
