#ifndef GANTTHEADERWIDGET_H
#define GANTTHEADERWIDGET_H

#include "scrollablewidget.h"
#include "application.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QRectF>
#include <QPointF>

class GanttHeaderWidget : public ScrollableWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor)
        Q_PROPERTY(QColor inactiveColor READ inactiveColor WRITE setInactiveColor)
        Q_PROPERTY(QColor activeForegroundColor READ activeForegroundColor WRITE setActiveForegroundColor)
        Q_PROPERTY(QColor inactiveForegroundColor READ inactiveForegroundColor WRITE setInactiveForegroundColor)

    public:
        explicit GanttHeaderWidget(QWidget *parent = nullptr);

        void setApplication(Application* app);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);

        void setMarkers(QMap<float, QList<GanttMarker*>>* markers);
        void setItems(QList<GanttItem*>* items);

        void setCellWidth(int width);
        void setCellBeats(float beats);

        void setPositionFunction(std::function<float(void)> func);
        void setHeaderPaintFunction(std::function<void(QPainter&,QRect,float,float,float)> func);

        void scrollBy(const int pixels);

        void setSnap(const bool snap);

        bool hasLoop() const;
        float loopStart() const;
        float loopEnd() const;

        const QColor& cursorColor() const;
        const QColor& selectionColor() const;

        void setCursorColor(const QColor& color);
        void setSelectionColor(const QColor& color);

    signals:
        void markerClicked(GanttMarker* marker);
        void clicked(Qt::MouseButton button, float time, const QPoint& location);
        void loopChanged();

    protected:
        int length() const;
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

    private:
        Application* _app;

        QList<GanttItem*>* _items;
        QMap<float, QList<GanttMarker*>>* _markers;

        int _left;

        float _loopStart;
        float _loopEnd;

        int _cellWidth;
        float _cellBeats;
        bool _snap;

        QColor _activeColor;
        QColor _inactiveColor;
        QColor _activeForegroundColor;
        QColor _inactiveForegroundColor;
        QColor _cursorColor;
        QColor _selectionColor;

        const QColor& activeColor() const;
        const QColor& inactiveColor() const;
        const QColor& activeForegroundColor() const;
        const QColor& inactiveForegroundColor() const;

        void setActiveColor(const QColor& color);
        void setInactiveColor(const QColor& color);
        void setActiveForegroundColor(const QColor& color);
        void setInactiveForegroundColor(const QColor& color);
        float playlength() const;

        std::function<float(void)> _positionFunction;
        std::function<void(QPainter& painter, QRect rect, float left, float right, float bpp)> _headerPaintFunction;

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);
};

#endif // GANTTHEADERWIDGET_H
