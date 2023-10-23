#ifndef GANTTEDITORWIDGET_H
#define GANTTEDITORWIDGET_H

#include "application.h"
#include "ganttwidget/ganttitem.h"
#include <QtMath>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class GanttEditorWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor lightBorderColor READ lightBorderColor WRITE setLightBorderColor)
        Q_PROPERTY(QColor itemColor READ itemColor WRITE setItemColor)
        Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor)

    public:
        explicit GanttEditorWidget(QWidget *parent = nullptr);

        void setVerticalScrollPercentage(const float percent);
        void setHorizontalScrollPercentage(const float percent);

        void setItems(QList<GanttItem*>* items);

        void setApplication(Application* app);

        void setSnap(const bool snap);
        void invertRows(const bool invert);

        void setCellWidth(int width);
        void setCellBeats(float beats);
        void setRows(int rows);
        void setRowHeight(int height);

        void setItemsResizable(const bool resizable);
        void setItemsMovableX(const bool movable);
        void setItemsMovableY(const bool movable);

        void setPositionFunction(std::function<float(void)> func);

    signals:
        void horizontalScroll(const int pixels);
        void verticalScroll(const int pixels);
        void clicked(Qt::MouseButton button, int row, float time);
        void itemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
        void itemReleased(const GanttItem* item);

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent*);
        void mouseMoveEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        QColor _backgroundColor;
        QColor _borderColor;
        QColor _lightBorderColor;
        QColor _itemColor;
        QColor _cursorColor;

        const QColor& backgroundColor() const;
        const QColor& borderColor() const;
        const QColor& lightBorderColor() const;
        const QColor& itemColor() const;
        const QColor& cursorColor() const;

        void setBackgroundColor(const QColor& color);
        void setBorderColor(const QColor& color);
        void setLightBorderColor(const QColor& color);
        void setItemColor(const QColor& color);
        void setCursorColor(const QColor& color);

        Application* _app;

        long _top;
        long _left;

        int _rows;
        int _rowHeight;

        int _cellWidth;
        float _cellBeats;

        bool _snap;
        bool _invertRows;

        QList<GanttItem*>* _items;

        GanttItem* _itemUnderCursor;
        int _cursorPositionOverItem;
        bool _itemUnderCursorSelected;

        bool _itemsResizable;
        bool _itemsMovableX;
        bool _itemsMovableY;

        std::function<float(void)> _positionFunction;

        float length() const;
        float visibleLength() const;
};

#endif // GANTTEDITORWIDGET_H
