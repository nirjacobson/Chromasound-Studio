#ifndef SCROLLABLEWIDGET_H
#define SCROLLABLEWIDGET_H

#include <QWidget>

class ScrollableWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit ScrollableWidget(QWidget *parent = nullptr);
        virtual void setScrollPercentage(const float percent) = 0;

    signals:

};

#endif // SCROLLABLEWIDGET_H
