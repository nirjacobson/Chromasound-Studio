#ifndef SCROLLABLEWIDGET_H
#define SCROLLABLEWIDGET_H

#include <QWidget>

class ScrollableWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit ScrollableWidget(QWidget *parent = nullptr);
        virtual float getScrollPercentage() = 0;
        virtual void setScrollPercentage(const float percent) = 0;
        virtual void scrollBy(const int pixels) = 0;

    protected:
        virtual int length() const = 0;
    signals:

};

#endif // SCROLLABLEWIDGET_H
