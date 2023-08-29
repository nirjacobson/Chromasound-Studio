#ifndef PLAYLISTPATTERNSWIDGET_H
#define PLAYLISTPATTERNSWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QtMath>

#include "ganttwidget/ganttleftwidget.h"

class PlaylistPatternsWidget : public GanttLeftWidget
{
        Q_OBJECT
    public:
        explicit PlaylistPatternsWidget(QWidget *parent = nullptr);

        void setScrollPercentage(const float percent);
        void setRows(int rows);
        void setRowHeight(int height);

    signals:

    protected:
        void paintEvent(QPaintEvent* event);

    private:
        int _rows;
        int _top;
        int _rowHeight;
};

#endif // PLAYLISTPATTERNSWIDGET_H
