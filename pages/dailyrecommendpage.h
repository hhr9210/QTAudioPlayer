#ifndef DAILYRECOMMENDPAGE_H
#define DAILYRECOMMENDPAGE_H

#include <QWidget>

class DailyRecommendPage : public QWidget
{
    Q_OBJECT

public:
    explicit DailyRecommendPage(QWidget *parent = nullptr);
    ~DailyRecommendPage() override;
};

#endif // DAILYRECOMMENDPAGE_H

