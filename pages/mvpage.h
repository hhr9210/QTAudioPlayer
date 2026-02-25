#ifndef MVPAGE_H
#define MVPAGE_H

#include <QWidget>
#include <QList>
#include <QPixmap>

struct VideoItem {
    QString fileName;
    QString filePath;
    QPixmap thumbnail;
    QString durationStr;
};

class MVPage : public QWidget
{
    Q_OBJECT

public:
    explicit MVPage(QWidget *parent = nullptr);
    ~MVPage() override;

signals:
    void videoClicked(const QString &filePath);

private:
    void setupUI();
    void loadLocalVideos();

    QWidget *m_contentWidget;
    QList<VideoItem> m_videos;
};

#endif // MVPAGE_H

