#ifndef RANKINGPAGE_H
#define RANKINGPAGE_H

#include <QWidget>

class RankingPage : public QWidget
{
    Q_OBJECT

public:
    explicit RankingPage(QWidget *parent = nullptr);
    ~RankingPage() override;
};

#endif // RANKINGPAGE_H

