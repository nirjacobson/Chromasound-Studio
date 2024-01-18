#ifndef GANTTBOTTOMWIDGET_H
#define GANTTBOTTOMWIDGET_H

#include "scrollablewidget.h"
#include "ganttitem.h"

class GanttBottomWidget : public ScrollableWidget
{
        Q_OBJECT
    public:
        explicit GanttBottomWidget(QWidget *parent = nullptr);

        virtual void setItems(QList<GanttItem*>* items) = 0;
        virtual void setCellWidth(const int width) = 0;
        virtual void setCellBeats(const float beats) = 0;
};

#endif // GANTTBOTTOMWIDGET_H
