#ifndef GANTTBOTTOMWIDGET_H
#define GANTTBOTTOMWIDGET_H

#include "scrollablewidget.h"
#include "ganttitem.h"

class GanttBottomWidget : public ScrollableWidget
{
        Q_OBJECT
    public:
        explicit GanttBottomWidget(QWidget *parent = nullptr);

        virtual void setCellWidth(const int width) = 0;
        virtual void setCellBeats(const float beats) = 0;
        virtual void setSnap(const bool enabled) = 0;
};

#endif // GANTTBOTTOMWIDGET_H
