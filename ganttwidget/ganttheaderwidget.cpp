#include "ganttheaderwidget.h"

constexpr QColor GanttHeaderWidget::BackgroundColor;
constexpr QColor GanttHeaderWidget::CursorColor;

GanttHeaderWidget::GanttHeaderWidget(QWidget *parent)
    : ScrollableWidget{parent}
    , _left(0)
    , _cellWidth(16)
    , _cellBeats(1)
{

}

void GanttHeaderWidget::setApplication(Application* app)
{
    _app = app;
}

void GanttHeaderWidget::setScrollPercentage(const float percent)
{
    int totalWidth = _cellWidth * _app->project().getLength() / _cellBeats;
    int scrollWidth = qMax(0, totalWidth - width());
    _left = percent * scrollWidth;

    update();
}

void GanttHeaderWidget::setCellWidth(int width)
{
    _cellWidth = width;
}

void GanttHeaderWidget::setCellBeats(float beats)
{
    _cellBeats = beats;
}

void GanttHeaderWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setPen(QColor(Qt::gray));
    painter.setBrush(QColor(BackgroundColor));

    int beatsPerBar = _app->project().beatsPerBar();

    int barWidth = _cellWidth / _cellBeats * beatsPerBar;
    int firstBar = _left / barWidth;
    int firstBarStart = firstBar * barWidth - _left;
    int numBars = qCeil((float)width()/barWidth);

    QPoint topLeft = QPoint(firstBarStart, 0);

    for (int i = 0; i < numBars; i++) {
        QPoint thisTopLeft = topLeft + QPoint(i * barWidth, 0);
        QPoint thisBottomRight = thisTopLeft + QPoint(barWidth, height());

        painter.setBrush(((firstBar + i) * beatsPerBar) < _app->project().getLength() ? QColor(BackgroundColor) : QColor(Qt::lightGray));
        painter.drawRect(QRect(thisTopLeft, thisBottomRight));

        QPoint thisBottomLeft = QPoint(thisTopLeft.x(), thisBottomRight.y());
        painter.drawText(thisBottomLeft + QPoint(4, -4), QString("%1").arg((firstBar + i) + 1));
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    float appPosition = _app->position();

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
