#ifndef PIANOROLLVELOCITIESWIDGET_H
#define PIANOROLLVELOCITIESWIDGET_H

#include <QPainter>
#include <QMouseEvent>

#include "ganttwidget/ganttitem.h"
#include "ganttwidget/ganttbottomwidget.h"
#include "application.h"
#include "commands/editnotecommand.h"

class PianoRollVelocitiesWidget : public GanttBottomWidget
{
        Q_OBJECT
    public:
        PianoRollVelocitiesWidget(QWidget *parent = nullptr);

        void setApplication(Application* app);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);
        void scrollBy(const int pixels);

        void setItems(QList<GanttItem*>* items);

        void setCellWidth(int width);
        void setCellBeats(float beats);

    protected:
        int length() const;
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);

    private:
        static constexpr QColor BarColor = QColor(192, 192, 255);

        Application* _app;
        QList<GanttItem*>* _items;
        int _left;

        int _cellWidth;
        float _cellBeats;

        float playlength() const;
};

#endif // PIANOROLLVELOCITIESWIDGET_H
