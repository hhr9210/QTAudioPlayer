#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QListWidget>

class MenuWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget() override;
};

#endif // MENUWIDGET_H

