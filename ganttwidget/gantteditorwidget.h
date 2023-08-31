#ifndef GANTTEDITORWIDGET_H
#define GANTTEDITORWIDGET_H

#include "application.h"
#include "ganttwidget/ganttitem.h"
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

        float length() const;
        void setLength(const float length);

    signals:
        void clicked(Qt::MouseButton button, int row, float time);

    protected:
        void paintEvent(QPaintEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

    private:
        static constexpr QColor CursorColor = QColor(64, 192, 64);
        static constexpr QColor ItemColor = QColor(128, 128, 255);

        Application* _app;

        long _top;
        long _left;

        int _rows;
        int _rowHeight;

        float _length;

        int _cellWidth;
        float _cellBeats;

        bool _snap;
        bool _invertRows;

        QList<GanttItem*>* _items;

        GanttItem* _itemUnderCursor;
        int _cursorPositionOverItem;
        bool _itemUnderCursorSelected;
};

#endif // GANTTEDITORWIDGET_H
