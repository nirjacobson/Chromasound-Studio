#ifndef GANTTEDITORWIDGET_H
#define GANTTEDITORWIDGET_H

#include "application.h"
#include "common/ganttwidget/ganttitem.h"
#include "common/damagewidget/damagewidget.h"
#include <QtMath>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>

class GanttEditorWidget : public DamageWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor itemColor READ itemColor WRITE setItemColor)
        Q_PROPERTY(QColor areaSelectionColor READ areaSelectionColor WRITE setAreaSelectionColor)

    public:
        explicit GanttEditorWidget(QWidget *parent = nullptr);

        void setVerticalScrollPercentage(const float percent);
        void setHorizontalScrollPercentage(const float percent);

        void setMarkers(QMap<float, QList<GanttMarker*>>* markers);
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

        const QList<GanttItem*>& selectedItems() const;
        void selectItems(const QList<GanttItem*>& items);
        void selectAllItems();

        void setCellMajors(const QList<int>& majors);
        void setRowMajors(const QList<int>& majors);

        float mousePosition() const;

        void setLoop(const float start, const float end);

        const QColor& cursorColor() const;
        const QColor& selectionColor() const;

        void setCursorColor(const QColor& color);
        void setSelectionColor(const QColor& color);

    signals:
        void horizontalScroll(const int pixels);
        void verticalScroll(const int pixels);
        void clicked(Qt::MouseButton button, int row, float time);
        void itemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
        void itemReleased(const GanttItem* item);
        void contextMenuRequested(GanttItem* item, const QPoint& location);

    protected:
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent*event);
        void mouseMoveEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        QColor _backgroundColor;
        QColor _borderColor;
        QColor _itemColor;
        QColor _cursorColor;
        QColor _areaSelectionColor;
        QColor _selectionColor;

        const QColor& backgroundColor() const;
        const QColor& borderColor() const;
        const QColor& itemColor() const;
        const QColor& areaSelectionColor() const;

        void setBackgroundColor(const QColor& color);
        void setBorderColor(const QColor& color);
        void setItemColor(const QColor& color);
        void setAreaSelectionColor(const QColor& color);

        Application* _app;

        float _mousePosition;

        long _top;
        long _left;

        float _loopStart;
        float _loopEnd;

        int _rows;
        int _rowHeight;

        int _cellWidth;
        float _cellBeats;
        QList<int> _cellMajors;
        QList<int> _rowMajors;

        bool _snap;
        bool _invertRows;

        QList<GanttItem*>* _items;
        QMap<float, QList<GanttMarker*>>* _markers;

        GanttItem* _itemUnderCursor;
        int _cursorPositionOverItem;

        bool _itemsResizable;
        bool _itemsMovableX;
        bool _itemsMovableY;

        bool _selecting;
        QPoint _fromPoint;
        QPoint _toPoint;
        QList<GanttItem*> _selectedItems;

        std::function<float(void)> _positionFunction;

        float length() const;
        float visibleLength() const;

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);

        // QWidget interface
    protected:
        void showEvent(QShowEvent* event);
};

#endif // GANTTEDITORWIDGET_H
