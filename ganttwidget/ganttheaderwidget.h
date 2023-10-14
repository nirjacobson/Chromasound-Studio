#ifndef GANTTHEADERWIDGET_H
#define GANTTHEADERWIDGET_H

#include "scrollablewidget.h"
#include "application.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

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
        void setHeaderPaintFunction(std::function<void(QPainter&,QRect,float,float,float)> func);

        void scrollBy(const int pixels);

        void setSnap(const bool snap);

    signals:
        void clicked(Qt::MouseButton button, float time);

    protected:
        int length() const;
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);

    private:
        static constexpr QColor BackgroundColor = QColor(64, 64, 64);
        static constexpr QColor CursorColor = QColor(64, 192, 64);

        Application* _app;

        QList<GanttItem*>* _items;

        int _left;

        int _cellWidth;
        float _cellBeats;
        bool _snap;

        float playlength() const;

        std::function<float(void)> _positionFunction;
        std::function<void(QPainter& painter, QRect rect, float left, float right, float bpp)> _headerPaintFunction;
};

#endif // GANTTHEADERWIDGET_H
