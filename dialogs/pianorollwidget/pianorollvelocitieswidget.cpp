#include "pianorollvelocitieswidget.h"

PianoRollVelocitiesWidget::PianoRollVelocitiesWidget(QWidget* parent)
    : GanttBottomWidget(parent)
    , _app(nullptr)
    , _items(nullptr)
    , _left(0)
    , _cellWidth(16)
    , _cellBeats(0.25)
    , _barColor(192, 192, 255)
{
    setMinimumHeight(128);
    setMaximumHeight(128);
}

void PianoRollVelocitiesWidget::setApplication(Application* app)
{
    _app = app;
}

float PianoRollVelocitiesWidget::getScrollPercentage()
{
    int scrollWidth = qMax(0, length() - width());

    if (scrollWidth == 0) return 0;

    return (float)_left / (float)scrollWidth;
}

void PianoRollVelocitiesWidget::setScrollPercentage(const float percent)
{
    int scrollWidth = qMax(0, length() - width());
    _left = percent * scrollWidth;

    setNeedsFullPaint();
    update();
}

void PianoRollVelocitiesWidget::scrollBy(const int pixels)
{
    _left += pixels;
    setNeedsFullPaint();
    update();
}

void PianoRollVelocitiesWidget::setItems(QList<GanttItem*>* items)
{
    _items = items;
}

void PianoRollVelocitiesWidget::setCellWidth(int width)
{
    _cellWidth = width;

    setNeedsFullPaint();
    update();
}

void PianoRollVelocitiesWidget::setCellBeats(float beats)
{
    _cellBeats = beats;
}

int PianoRollVelocitiesWidget::length() const
{
    int visibleLength = ((int)(playlength() / _app->project().beatsPerBar()) + 1) * _app->project().beatsPerBar();
    int totalWidth = _cellWidth * visibleLength / _cellBeats;

    return totalWidth;
}

void PianoRollVelocitiesWidget::mousePressEvent(QMouseEvent* event)
{
    if (_items) {
        int barWidth = 8;
        int fullBarHeight = height() - 16;
        float beatsPerPixel = _cellBeats / _cellWidth;

        float mousePosition = (_left + event->position().x()) * beatsPerPixel;
        int invertY = height() - event->position().y();
        int velocityClicked = qMin(100, (int)((float)invertY/(float)fullBarHeight * 100));

        auto it = std::find_if(_items->begin(), _items->end(), [=](GanttItem* const item) {
            return item->time() <= mousePosition && mousePosition <= item->time() + barWidth * beatsPerPixel;
        });

        if (it != _items->end()) {
            Track::Item* item = dynamic_cast<Track::Item*>(*it);
            Note n = item->note();
            n.setVelocity(velocityClicked);
            _app->undoStack().push(new EditNoteCommand(_app->window(), item, item->time(), n, QList<Track::Item*>()));
        }
    }
}

const QColor& PianoRollVelocitiesWidget::barColor() const
{
    return _barColor;
}

void PianoRollVelocitiesWidget::setBarColor(const QColor& color)
{
    _barColor = color;
}

float PianoRollVelocitiesWidget::playlength() const
{
    float end = 0;
    if (_items) {
        for (const GanttItem* item : *_items) {
            float thisEnd = item->time() + item->duration();
            if (thisEnd > end) {
                end = thisEnd;
            }
        }
    }

    return end;
}

void PianoRollVelocitiesWidget::paintFull(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(_barColor.darker());
    painter.setBrush(_barColor);

    int barWidth = 8;

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    int fullBarHeight = height() - 16;

    if (_items) {
        QList<QRect> rects;
        for (const GanttItem* item : *_items) {
            if (leftPosition <= item->time() && item->time() <= rightPosition) {
                int positionPixel = (item->time() - leftPosition) / beatsPerPixel;
                QPoint topLeft = QPoint(positionPixel, height() - fullBarHeight * (float)item->velocity()/float(100));
                rects.append(QRect(topLeft, topLeft + QPoint(barWidth, height() - topLeft.y())));
            }
        }
        std::sort(rects.begin(), rects.end(), [](QRect& a, QRect& b) {
            return a.height() > b.height();
        });

        for (const QRect& rect : rects) {
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);
        }
    }
}

void PianoRollVelocitiesWidget::paintPartial(QPaintEvent* event)
{

}

void PianoRollVelocitiesWidget::setSnap(const bool enabled)
{

}
