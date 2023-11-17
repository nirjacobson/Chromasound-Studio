#include "ganttheaderwidget.h"

GanttHeaderWidget::GanttHeaderWidget(QWidget *parent)
    : ScrollableWidget{parent}
    , _items(nullptr)
    , _markers(nullptr)
    , _left(0)
    , _loopStart(-1)
    , _loopEnd(-1)
    , _cellWidth(16)
    , _cellBeats(1)
    , _snap(true)
    , _positionFunction([](){ return -1; })
    , _headerPaintFunction([](QPainter&,QRect,float,float,float){})
    , _activeColor(QColor(64, 64, 64))
    , _inactiveColor(Qt::lightGray)
    , _activeForegroundColor(Qt::gray)
    , _inactiveForegroundColor(Qt::gray)
    , _cursorColor(QColor(64, 192, 64))
    , _loopColor(QColor(255, 192, 0))
    , _markerColor(QColor(255, 128, 128))
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

void GanttHeaderWidget::setMarkers(QList<GanttMarker*>* markers)
{
    _markers = markers;
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

void GanttHeaderWidget::setHeaderPaintFunction(std::function<void (QPainter&, QRect, float, float, float)> func)
{
    _headerPaintFunction = func;
}

void GanttHeaderWidget::scrollBy(const int pixels)
{
    _left += pixels;
    update();
}

void GanttHeaderWidget::setSnap(const bool snap)
{
    _snap = snap;
}

bool GanttHeaderWidget::hasLoop() const
{
    return _loopStart >=0 && _loopEnd >=0;
}

float GanttHeaderWidget::loopStart() const
{
    return _loopStart;
}

float GanttHeaderWidget::loopEnd() const
{
    return _loopEnd;
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

    int beatsPerBar = _app->project().beatsPerBar();

    int barWidth = _cellWidth / _cellBeats * beatsPerBar;
    int firstBar = _left / barWidth;
    int firstBarStart = firstBar * barWidth - _left;
    int numBars = qCeil((float)width()/(float)barWidth) + 1;

    QPoint topLeft = QPoint(firstBarStart, 0);

    for (int i = 0; i < numBars; i++) {
        QPoint thisTopLeft = topLeft + QPoint(i * barWidth, 0);
        QPoint thisBottomRight = thisTopLeft + QPoint(barWidth, height());
        bool active = ((firstBar + i) * beatsPerBar) < playlength();

        painter.setPen(active ? _activeForegroundColor : _inactiveForegroundColor);
        painter.setBrush(active ? _activeColor : _inactiveColor);
        painter.drawRect(QRect(thisTopLeft, thisBottomRight));

        QPoint thisBottomLeft = QPoint(thisTopLeft.x(), thisBottomRight.y());
        painter.drawText(thisBottomLeft + QPoint(4, -4), QString("%1").arg((firstBar + i) + 1));
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    _headerPaintFunction(painter, rect(), leftPosition, rightPosition, beatsPerPixel);

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
        painter.fillPath(path, _cursorColor);
    }

    if (_markers) {
        for (GanttMarker* marker : *_markers) {
            QRectF br = painter.boundingRect(rect(), marker->name());

            if (marker->time() + (br.width() * beatsPerPixel) > leftPosition) {
                int markerPixel = (marker->time() - leftPosition) / beatsPerPixel;

                painter.setPen(_markerColor);
                painter.setBrush(_markerColor);
                br = QRectF(QPoint(markerPixel, 0), QSize(br.width() + 8, height()));
                painter.drawRect(br);
                painter.setPen(Qt::white);
                painter.drawText(br.adjusted(4, 0, 0, -4), Qt::AlignLeft | Qt::AlignBottom, marker->name());
            }
        }
    }

    if (hasLoop()) {
        if (leftPosition <= _loopEnd && _loopStart <= rightPosition) {
            int loopFromPixel = (_loopStart - leftPosition) / beatsPerPixel;
            int width = (_loopEnd - _loopStart) / beatsPerPixel;


            QColor loopColorWithAlpha = _loopColor;
            loopColorWithAlpha.setAlpha(128);
            painter.setPen(loopColorWithAlpha.darker());
            painter.setBrush(loopColorWithAlpha);
            painter.drawRect(QRect(QPoint(loopFromPixel, 0), QSize(width, height() - 1)));
        }
    }
}

void GanttHeaderWidget::mousePressEvent(QMouseEvent* event)
{
    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float mousePosition = leftPosition + (event->pos().x() * beatsPerPixel);
    float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;

    _loopStart = _snap ? mousePositionSnapped : mousePosition;
    _loopEnd = -1;

    update();
    emit loopChanged();

    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        if (_markers) {
            for (GanttMarker* marker : *_markers) {
                if (marker->time() <= mousePosition && mousePosition < marker->time() + (height() * beatsPerPixel)) {
                    emit markerClicked(marker);
                    return;
                }
            }
        }
    }

    emit clicked(event->button(), _snap ? mousePositionSnapped : mousePosition);
}

void GanttHeaderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (_loopStart > _loopEnd) {
        float temp = _loopStart;
        _loopStart = _loopEnd;
        _loopEnd = temp;
        emit loopChanged();
    }
}

void GanttHeaderWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        float beatsPerPixel = _cellBeats / _cellWidth;
        float leftPosition = _left * beatsPerPixel;
        float mousePosition = leftPosition + (event->pos().x() * beatsPerPixel);
        float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;

        _loopEnd = _snap ? mousePositionSnapped : mousePosition;
        update();

        emit loopChanged();
    }
}

const QColor& GanttHeaderWidget::activeColor() const
{
    return _activeColor;
}

const QColor& GanttHeaderWidget::inactiveColor() const
{
    return _inactiveColor;
}

const QColor& GanttHeaderWidget::activeForegroundColor() const
{
    return _activeForegroundColor;
}

const QColor& GanttHeaderWidget::inactiveForegroundColor() const
{
    return _inactiveColor;
}

const QColor& GanttHeaderWidget::cursorColor() const
{
    return _cursorColor;
}

const QColor& GanttHeaderWidget::loopColor() const
{
    return _loopColor;
}

void GanttHeaderWidget::setActiveColor(const QColor& color)
{
    _activeColor = color;
}

void GanttHeaderWidget::setInactiveColor(const QColor& color)
{
    _inactiveColor = color;
}

void GanttHeaderWidget::setActiveForegroundColor(const QColor& color)
{
    _activeForegroundColor = color;
}

void GanttHeaderWidget::setInactiveForegroundColor(const QColor& color)
{
    _inactiveForegroundColor = color;
}

void GanttHeaderWidget::setCursorColor(const QColor& color)
{
    _cursorColor = color;
}

void GanttHeaderWidget::setLoopColor(const QColor& color)
{
    _loopColor = color;
}

int GanttHeaderWidget::length() const
{
    int visibleLength = ((int)(playlength() / _app->project().beatsPerBar()) + 1) * _app->project().beatsPerBar();
    int totalWidth = _cellWidth * visibleLength / _cellBeats;

    return totalWidth;
}
