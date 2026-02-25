#ifndef SEARCHRESULTPAGE_H
#define SEARCHRESULTPAGE_H

#include <QWidget>
#include <QList>
#include "../core/musicapimanager.h"

class QVBoxLayout;
class QLabel;

class SearchResultPage : public QWidget
{
    Q_OBJECT

public:
    explicit SearchResultPage(QWidget *parent = nullptr);
    ~SearchResultPage();

    void setSearchResults(const QList<SongItem> &songs);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void songDoubleClicked(const SongItem &song);

private:
    void setupUI();

    QWidget *m_resultsContainer;
    QLabel *m_titleLabel;
    QList<SongItem> m_songs;
};

#endif // SEARCHRESULTPAGE_H
