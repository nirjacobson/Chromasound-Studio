#ifndef PIANOROLLVELOCITIESWIDGET_H
#define PIANOROLLVELOCITIESWIDGET_H

#include <QPainter>
#include <QMouseEvent>

#include "common/ganttwidget/ganttitem.h"
#include "common/ganttwidget/ganttbottomwidget.h"
#include "application.h"
#include "commands/editnotecommand.h"

class PianoRollVelocitiesWidget : public GanttBottomWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor barColor READ barColor WRITE setBarColor)

    public:
        PianoRollVelocitiesWidget(QWidget *parent = nullptr);

        void setApplication(Application* app);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);
        void scrollBy(const int pixels);

        void setItems(QList<GanttItem*>* items);

        void setCellWidth(int width);
        void setCellBeats(float beats);

        void setDefaultPlaylength(const float length);

    protected:
        int length() const;
        void mousePressEvent(QMouseEvent* event);

    private:
        QColor _barColor;

        const QColor& barColor() const;

        void setBarColor(const QColor& color);

        Application* _app;
        QList<GanttItem*>* _items;
        int _left;

        float _defaultPlaylength;

        int _cellWidth;
        float _cellBeats;

        float playlength() const;

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);

        // GanttBottomWidget interface
    public:
        void setSnap(const bool enabled);
};

#endif // PIANOROLLVELOCITIESWIDGET_H
