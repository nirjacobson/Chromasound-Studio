#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include <QWidget>

#include "application.h"
#include "ganttleftwidget.h"
#include "ganttbottomwidget.h"
#include "ganttitem.h"
#include "ganttmarker.h"

namespace Ui {
    class GanttWidget;
}

class GanttWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor)
        Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor)

    public:
        explicit GanttWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~GanttWidget();

        void setLeftWidget(GanttLeftWidget* const widget);
        void setBottomWidget(GanttBottomWidget* const widget);
        void setParameters(int rows, int rowHeight, int cellWidth, float beatsPerCell);

        void addMarkers(QList<GanttMarker*>* markers);
        void setItems(QList<GanttItem*>* items);
        void setApplication(Application* app);
        void invertRows(const bool invert);

        void setItemsResizable(const bool resizable);
        void setItemsMovableX(const bool movable);
        void setItemsMovableY(const bool movable);

        void setPositionFunction(std::function<float(void)> func);
        void setHeaderPaintFunction(std::function<void (QPainter&, QRect, float, float, float)> func);

        void scrollVertically(const float percentage);

        const QList<GanttItem*>& selectedItems() const;
        void selectItems(const QList<GanttItem*>& items);
        void selectAllItems();

        void setCellWidth(const int width);

        void setCellMajors(const QList<int>& majors);
        void setRowMajors(const QList<int>& majors);

        float mousePosition() const;

        bool hasLoop() const;
        float loopStart() const;
        float loopEnd() const;

        void setDefaultCellWidth(int width);

        void doUpdate();

    private:
        Ui::GanttWidget *ui;
        Application* _app;

        QList<QList<GanttMarker*>*> _markers;
        QMap<float, QList<GanttMarker*>> _markersMap;

        GanttLeftWidget* _leftWidget;
        GanttBottomWidget* _bottomWidget;

        int _rows;
        int _rowHeight;
        int _cellWidth;
        float _cellBeats;
        int _defaultCellWidth;

        const QColor& cursorColor() const;
        const QColor& selectionColor() const;

        void setCursorColor(const QColor& color);
        void setSelectionColor(const QColor& color);

        void rebuildMarkersMap();

    private slots:
        void verticalScroll(int amount);
        void horizontalScroll(int amount);
        void snapClicked();
        void wheelHorizontalScroll(int pixels);
        void wheelVerticalScroll(int pixels);
        void loopChanged();
        void scaleChanged(float scale);

    signals:
        void markerClicked(GanttMarker* marker);
        void headerClicked(Qt::MouseButton button, float time, const QPoint& location);
        void editorClicked(Qt::MouseButton button, int row, float time);
        void itemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
        void itemReleased(const GanttItem* item);
        void contextMenuRequested(GanttItem* item, const QPoint& location);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // GANTTWIDGET_H
