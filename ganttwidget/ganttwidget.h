#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include <QWidget>

#include "../application.h"
#include "ganttleftwidget.h"
#include "ganttbottomwidget.h"
#include "ganttitem.h"

namespace Ui {
    class GanttWidget;
}

class GanttWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit GanttWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~GanttWidget();

        void setLeftWidget(GanttLeftWidget* const widget);
        void setBottomWidget(GanttBottomWidget* const widget);
        void setParameters(int rows, int rowHeight, int cellWidth, float beatsPerCell);

        void setItems(QList<GanttItem*>* items);
        void setApplication(Application* app);
        void invertRows(const bool invert);

        void setItemsResizable(const bool resizable);
        void setItemsMovableX(const bool movable);
        void setItemsMovableY(const bool movable);

        void setPositionFunction(std::function<float(void)> func);

        void scrollVertically(const float percentage);

    private:
        Ui::GanttWidget *ui;
        Application* _app;

        GanttLeftWidget* _leftWidget;
        GanttBottomWidget* _bottomWidget;

        int _rows;
        int _rowHeight;
        int _cellWidth;
        float _cellBeats;

    private slots:
        void verticalScroll(int amount);
        void horizontalScroll(int amount);
        void snapClicked();
        void wheelHorizontalScroll(int pixels);
        void wheelVerticalScroll(int pixels);

    signals:
        void headerClicked(Qt::MouseButton button, float time);
        void editorClicked(Qt::MouseButton button, int row, float time);
        void itemsChanged();
        void itemReleased(const GanttItem* item);
};

#endif // GANTTWIDGET_H
