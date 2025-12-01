#ifndef PIANOROLLPITCHWIDGET_H
#define PIANOROLLPITCHWIDGET_H

#include "common/ganttwidget/ganttbottomwidget.h"
#include "project/track.h"
#include "application.h"
#include "commands/editpitchchangecommand.h"

class PianoRollPitchWidget : public GanttBottomWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(QColor itemColor READ itemColor WRITE setItemColor)
    Q_PROPERTY(QColor areaSelectionColor READ areaSelectionColor WRITE setAreaSelectionColor)


protected:
    int length() const;

public:
    PianoRollPitchWidget(QWidget* parent = nullptr);

    void setApplication(Application* app);

    const QList<Track::PitchChange*>& selectedItems();

    float getScrollPercentage();
    void setScrollPercentage(const float percent);
    void scrollBy(const int pixels);

    void setDefaultPlaylength(const float length);

    void setItems(QList<Track::PitchChange*>* items, const int pitchRange);
    void setCellWidth(const int width);
    void setCellBeats(const float beats);

private:
    void paintFull(QPaintEvent *event);
    void paintPartial(QPaintEvent *event);

    QColor _backgroundColor;
    QColor _borderColor;
    QColor _color;
    QColor _areaSelectionColor;
    QColor _selectionColor;

    float _mousePosition;

    int _left;

    bool _snap;

    float _defaultPlaylength;

    int _cellWidth;
    float _cellBeats;

    Application* _app;
    QList<Track::PitchChange*>* _items;
    int _pitchRange;

    Track::PitchChange* _itemUnderCursor;

    bool _selecting;
    QPoint _fromPoint;
    QPoint _toPoint;
    QList<Track::PitchChange*> _selectedItems;

    const QColor& backgroundColor() const;
    const QColor& borderColor() const;
    const QColor& itemColor() const;
    const QColor& areaSelectionColor() const;

    void setBackgroundColor(const QColor& color);
    void setBorderColor(const QColor& color);
    void setItemColor(const QColor& color);
    void setAreaSelectionColor(const QColor& color);
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void pitchChangeAdded(float time, float pitch);
    void pitchChangeRemoved(Track::PitchChange* change);

    // GanttBottomWidget interface
public:
    void setSnap(const bool enabled);
};

#endif // PIANOROLLPITCHWIDGET_H
