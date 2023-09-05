#ifndef GANTTHEADERWIDGET_H
#define GANTTHEADERWIDGET_H

#include "scrollablewidget.h"
#include "application.h"

#include <QPainter>
#include <QPainterPath>

class GanttHeaderWidget : public ScrollableWidget
{
        Q_OBJECT
    public:
        explicit GanttHeaderWidget(QWidget *parent = nullptr);

        void setApplication(Application* app);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);

        void setItems(QList<GanttItem*>* items);

        void setCellWidth(int width);
        void setCellBeats(float beats);

        void setPositionFunction(std::function<float(void)> func);

        void scrollBy(const int pixels);

    signals:

    protected:
        int length() const;
        void paintEvent(QPaintEvent* event);

    private:
        static constexpr QColor BackgroundColor = QColor(64, 64, 64);
        static constexpr QColor CursorColor = QColor(64, 192, 64);

        Application* _app;

        QList<GanttItem*>* _items;

        int _left;

        int _cellWidth;
        float _cellBeats;

        float playlength() const;

        std::function<float(void)> _positionFunction;
};

#endif // GANTTHEADERWIDGET_H
