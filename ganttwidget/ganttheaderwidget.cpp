#include "ganttheaderwidget.h"

constexpr QColor GanttHeaderWidget::BackgroundColor;
constexpr QColor GanttHeaderWidget::CursorColor;

GanttHeaderWidget::GanttHeaderWidget(QWidget *parent)
    : ScrollableWidget{parent}
    , _items(nullptr)
    , _left(0)
    , _cellWidth(16)
    , _cellBeats(1)
    , _positionFunction([](){ return -1; })
{

}

void GanttHeaderWidget::setApplication(Application* app)
{
    _app = app;
}

float GanttHeaderWidget::getScrollPercentage()
{
    int scrollWidth = qMax(0, length() - width());

    if (scrollWidth == 0) return 0;

    return (float)_left / (float)scrollWidth;
}

void GanttHeaderWidget::setScrollPercentage(const float percent)
{
    int scrollWidth = qMax(0, length() - width());
    _left = percent * scrollWidth;

    update();
}

void GanttHeaderWidget::setItems(QList<GanttItem*>* items)
{
    _items = items;
}

void GanttHeaderWidget::setCellWidth(int width)
{
    _cellWidth = width;
}

void GanttHeaderWidget::setCellBeats(float beats)
{
    _cellBeats = beats;
}

void GanttHeaderWidget::setPositionFunction(std::function<float ()> func)
{
    _positionFunction = func;
}

void GanttHeaderWidget::scrollBy(const int pixels)
{
    _left += pixels;
    update();
}

float GanttHeaderWidget::playlength() const
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

void GanttHeaderWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.setPen(QColor(Qt::gray));
    painter.setBrush(QColor(BackgroundColor));

    int beatsPerBar = _app->project().beatsPerBar();

    int barWidth = _cellWidth / _cellBeats * beatsPerBar;
    int firstBar = _left / barWidth;
    int firstBarStart = firstBar * barWidth - _left;
    int numBars = qCeil((float)width()/(float)barWidth) + 1;

    QPoint topLeft = QPoint(firstBarStart, 0);

    for (int i = 0; i < numBars; i++) {
        QPoint thisTopLeft = topLeft + QPoint(i * barWidth, 0);
        QPoint thisBottomRight = thisTopLeft + QPoint(barWidth, height());

        painter.setBrush(((firstBar + i) * beatsPerBar) < playlength() ? QColor(BackgroundColor) : QColor(Qt::lightGray));
        painter.drawRect(QRect(thisTopLeft, thisBottomRight));

        QPoint thisBottomLeft = QPoint(thisTopLeft.x(), thisBottomRight.y());
        painter.drawText(thisBottomLeft + QPoint(4, -4), QString("%1").arg((firstBar + i) + 1));
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    float appPosition = _positionFunction();

    if (leftPosition <= appPosition && appPosition <= rightPosition) {
        int appPositionPixel = (appPosition - leftPosition) / beatsPerPixel;

        QPainterPath path;

        QPoint p1(appPositionPixel - height() / 2, 0);
        QPoint p2(appPositionPixel, height() / 2);
        QPoint p3(appPositionPixel + height() / 2, 0);

        path.moveTo(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        path.lineTo(p1);

        painter.setPen(Qt::NoPen);
        painter.fillPath(path, CursorColor);
    }
}

int GanttHeaderWidget::length() const
{
    int visibleLength = ((int)(playlength() / _app->project().beatsPerBar()) + 1) * _app->project().beatsPerBar();
    int totalWidth = _cellWidth * visibleLength / _cellBeats;

    return totalWidth;
}
