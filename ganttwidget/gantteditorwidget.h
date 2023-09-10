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
        void itemsChanged();
        void itemReleased(const GanttItem* item);

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent*);
        void mouseMoveEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        static constexpr QColor CursorColor = QColor(64, 192, 64);
        static constexpr QColor ItemColor = QColor(128, 128, 255);

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
