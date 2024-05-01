#ifndef PLAYLISTPATTERNSWIDGET_H
#define PLAYLISTPATTERNSWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QtMath>
#include <QMouseEvent>
#include <QInputDialog>

#include "common/ganttwidget/ganttleftwidget.h"
#include "commands/setpatternnamecommand.h"
#include "application.h"

class PlaylistPatternsWidget : public GanttLeftWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor ledColor READ ledColor WRITE setLEDColor);

    public:
        explicit PlaylistPatternsWidget(QWidget *parent = nullptr, Application* app = nullptr);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);
        void scrollBy(const int pixels);
        void setRows(int rows);
        void setRowHeight(int height);

        void doUpdate(const float position);

    signals:
        void patternClicked(const int pattern);

    protected:
        int length() const;
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);

    private:
        static constexpr int LED_WIDTH = 8;

        QColor _ledColor;

        const QColor& ledColor() const;

        void setLEDColor(const QColor& color);

        Application* _app;
        float _appPosition;

        int _rows;
        int _top;
        int _rowHeight;
};

#endif // PLAYLISTPATTERNSWIDGET_H
