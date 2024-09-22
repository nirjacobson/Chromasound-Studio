#include "ganttheaderwidget.h"

GanttHeaderWidget::GanttHeaderWidget(QWidget *parent)
    : ScrollableWidget{parent}
    , _items(nullptr)
    , _left(0)
    , _loopStart(-1)
    , _loopEnd(-1)
    , _cellWidth(16)
    , _cellBeats(1)
    , _snap(true)
    , _markers(nullptr)
    , _positionFunction([]() {
    return -1;
})
, _headerPaintFunction([](QPainter&,QRect,float,float,float) {})
, _activeColor(QColor(64, 64, 64))
, _inactiveColor(Qt::lightGray)
, _activeForegroundColor(Qt::gray)
, _inactiveForegroundColor(Qt::gray)
, _cursorColor(QColor(64, 192, 64))
, _selectionColor(QColor(255, 192, 0))
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

    setNeedsFullPaint();
    update();
}

void GanttHeaderWidget::setMarkers(QMap<float, QList<GanttMarker*> >* markers)
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

    setNeedsFullPaint();
    update();
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
    setNeedsFullPaint();
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

void GanttHeaderWidget::paintFull(QPaintEvent* event)
{
    int minHeight = 24;
    int max = 0;
    if (_markers) {
        for (auto it = _markers->begin(); it != _markers->end(); ++it) {
            if (it->size() > max) {
                max = it->size();
            }
        }
    }
    minHeight += max * 24;

    setMinimumHeight(minHeight);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

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

        QPoint textBottomLeft = QPoint(thisTopLeft.x(), 24);
        painter.drawText(textBottomLeft + QPoint(4, -4), QString("%1").arg((firstBar + i) + 1));
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    _headerPaintFunction(painter, rect(), leftPosition, rightPosition, beatsPerPixel);

    if (_markers) {
        for (auto it = _markers->begin(); it != _markers->end(); ++it) {
            for (int i = 0; i < it->size(); i++) {
                GanttMarker* marker = (*it)[i];

                QRectF br = painter.boundingRect(rect(), marker->name());

                if (marker->time() + (br.width() * beatsPerPixel) > leftPosition) {
                    int markerPixel = (marker->time() - leftPosition) / beatsPerPixel;

                    painter.setPen(marker->color());
                    painter.setBrush(marker->color());
                    br = QRectF(QPoint(markerPixel, height() - (24 * (i + 1))), QSize(br.width() + 8, 24));
                    painter.drawRect(br);
                    painter.setPen(marker->color().lightness() < 96 ? Qt::white : Qt::black);
                    painter.drawText(br.adjusted(4, 0, 0, -4), Qt::AlignLeft | Qt::AlignBottom, marker->name());
                }
            }
        }

        if (hasLoop()) {
            if (leftPosition <= _loopEnd && _loopStart <= rightPosition) {
                int loopFromPixel = (_loopStart - leftPosition) / beatsPerPixel;
                int width = (_loopEnd - _loopStart) / beatsPerPixel;


                QColor loopColorWithAlpha = _selectionColor;
                loopColorWithAlpha.setAlpha(128);
                painter.setPen(loopColorWithAlpha.darker());
                painter.setBrush(loopColorWithAlpha);
                painter.drawRect(QRect(QPoint(loopFromPixel, 0), QSize(width, height() - 1)));
            }
        }
    }
}

void GanttHeaderWidget::paintPartial(QPaintEvent* event)
{
    QPainter painter(this);

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    float appPosition = _positionFunction();

    if (leftPosition <= appPosition && appPosition <= rightPosition) {
        int appPositionPixel = (appPosition - leftPosition) / beatsPerPixel;

        QPainterPath path;

        QPoint p1(appPositionPixel - 24 / 2, 0);
        QPoint p2(appPositionPixel, 24 / 2);
        QPoint p3(appPositionPixel + 24 / 2, 0);

        path.moveTo(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        path.lineTo(p1);

        painter.setPen(Qt::NoPen);
        painter.fillPath(path, _cursorColor);

        QPoint p4(appPositionPixel, 0);
        QPoint p5(appPositionPixel, height());

        painter.setPen(_cursorColor);
        painter.drawLine(p4, p5);
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

    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        if (_markers) {
            for (auto it = _markers->begin(); it != _markers->end(); ++it) {
                for (int i = 0; i < it->size(); i++) {
                    if (event->pos().y() <= (height() - (i * 24)) && event->pos().y() > (height() - ((i + 1) * 24))) {
                        if ((*it)[i]->time() <= mousePosition && mousePosition < (*it)[i]->time() + (24 * beatsPerPixel)) {
                            emit markerClicked((*it)[i]);
                            return;
                        }
                    }
                }
            }
        }
    }

    emit clicked(event->button(), _snap ? mousePositionSnapped : mousePosition, mapToGlobal(event->pos()));
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
        setNeedsFullPaint();
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

const QColor& GanttHeaderWidget::selectionColor() const
{
    return _selectionColor;
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

void GanttHeaderWidget::setSelectionColor(const QColor& color)
{
    _selectionColor = color;
}

int GanttHeaderWidget::length() const
{
    int visibleLength = ((int)(playlength() / _app->project().beatsPerBar()) + 1) * _app->project().beatsPerBar();
    int totalWidth = _cellWidth * visibleLength / _cellBeats;

    return totalWidth;
}
