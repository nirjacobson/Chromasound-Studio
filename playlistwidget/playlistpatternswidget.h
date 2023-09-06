#ifndef PLAYLISTPATTERNSWIDGET_H
#define PLAYLISTPATTERNSWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QtMath>

#include "ganttwidget/ganttleftwidget.h"
#include "application.h"

class PlaylistPatternsWidget : public GanttLeftWidget
{
        Q_OBJECT
    public:
        explicit PlaylistPatternsWidget(QWidget *parent = nullptr, Application* app = nullptr);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);
        void scrollBy(const int pixels);
        void setRows(int rows);
        void setRowHeight(int height);

    signals:

    protected:
        int length() const;
        void paintEvent(QPaintEvent* event);

    private:
        static constexpr int LED_WIDTH = 8;
        static QColor LED_COLOR;

        Application* _app;

        int _rows;
        int _top;
        int _rowHeight;

};

#endif // PLAYLISTPATTERNSWIDGET_H
