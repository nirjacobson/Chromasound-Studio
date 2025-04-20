#ifndef PIANOROLLPITCHWIDGET_H
#define PIANOROLLPITCHWIDGET_H

#include "common/ganttwidget/ganttbottomwidget.h"
#include "project/track.h"
#include "application.h"
#include "commands/editpitchchangecommand.h"

class PianoRollPitchWidget : public GanttBottomWidget
{
    Q_OBJECT

public:
    PianoRollPitchWidget(QWidget* parent = nullptr);

    void setApplication(Application* app);

    // DamageWidget interface
private:
    void paintFull(QPaintEvent *event);
    void paintPartial(QPaintEvent *event);

    // ScrollableWidget interface
public:
    float getScrollPercentage();
    void setScrollPercentage(const float percent);
    void scrollBy(const int pixels);

protected:
    int length() const;

    // GanttBottomWidget interface
public:
    void setItems(QList<Track::PitchChange*>* items, const int pitchRange);
    void setCellWidth(const int width);
    void setCellBeats(const float beats);

private:
    QColor _color;

    int _left;

    int _cellWidth;
    float _cellBeats;

    Application* _app;
    QList<Track::PitchChange*>* _items;
    int _pitchRange;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void pitchChangeAdded(float time, float pitch);
    void pitchChangeRemoved(Track::PitchChange* change);
};

#endif // PIANOROLLPITCHWIDGET_H
