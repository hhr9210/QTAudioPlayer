#ifndef SCROLLINGLABEL_H
#define SCROLLINGLABEL_H

#include <QLabel>
#include <QTimer>


//歌曲名自动滚动控件
class ScrollingLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ScrollingLabel(QWidget *parent = nullptr);
    ~ScrollingLabel() override;

    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateScroll();

private:
    QString m_fullText;
    int m_scrollPosition;
    QTimer *m_scrollTimer;
    bool m_needsScrolling;
    
    void checkScrolling();
};

#endif // SCROLLINGLABEL_H

