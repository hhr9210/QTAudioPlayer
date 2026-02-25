#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include <QFrame>
#include <QPushButton>
#include <QLineEdit>

class HeaderWidget : public QFrame
{
    Q_OBJECT

public:
    explicit HeaderWidget(QWidget *parent = nullptr);
    ~HeaderWidget() override;

    QPushButton *backButton() const { return m_backBtn; }
    QLineEdit *searchEdit() const { return m_searchEdit; }
    
    void setBackEnabled(bool enabled);

signals:
    void backClicked();
    void searchTextChanged(const QString &text);
    void searchRequested(const QString &text);

private:
    QPushButton *m_backBtn;
    QPushButton *m_forwardBtn;
    QLineEdit *m_searchEdit;
};

#endif // HEADERWIDGET_H

