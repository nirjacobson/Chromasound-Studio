#ifndef GANTTLEFTWIDGET_H
#define GANTTLEFTWIDGET_H

#include "scrollablewidget.h"

class GanttLeftWidget : public ScrollableWidget
{
        Q_OBJECT

    public:
        explicit GanttLeftWidget(QWidget *parent = nullptr);
        virtual void setRows(int rows) = 0;
        virtual void setRowHeight(int height) = 0;
};

#endif // GANTTLEFTWIDGET_H
