#include "pianorollpitchwidget.h"

PianoRollPitchWidget::PianoRollPitchWidget(QWidget *parent)
    : GanttBottomWidget(parent)
    , _left(0)
    , _cellWidth(16)
    , _cellBeats(1)
    , _color(QColor(128, 128, 255))
    , _app(nullptr)
    , _items(nullptr)
    , _itemUnderCursor(nullptr)
    , _backgroundColor(Qt::white)
    , _borderColor(Qt::gray)
    , _areaSelectionColor(QColor(192, 192, 255))
    , _selectionColor(QColor(255, 192, 0))
    , _selecting(false)
    , _selectedItems(QList<Track::PitchChange*>())
    , _snap(true)
{
    setMinimumHeight(128);
    setMaximumHeight(128);
    setMouseTracking(true);
}

void PianoRollPitchWidget::setApplication(Application *app)
{
    _app = app;
}

void PianoRollPitchWidget::paintFull(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QBrush(_backgroundColor));

    painter.drawLine(QPoint(0, height()/2), QPoint(width(), height() / 2));

    painter.setPen(_borderColor);
    painter.drawRect(rect());

    int firstCell = _left / _cellWidth;
    int firstCellStart = firstCell * _cellWidth - _left;
    int numCells = qCeil((float)width()/_cellWidth);

    QPoint topLeft(firstCellStart, 0);

    for (int i = 0; i < numCells; i++) {
        QPoint thisTopRight = topLeft + QPoint((i + 1) * _cellWidth, 0);
        QPoint thisBottomRight = thisTopRight + QPoint(0, height());

        painter.setPen(_borderColor.lighter(120));
        painter.drawLine(thisTopRight, thisBottomRight);
    }

    if (_items) {
        int pointRadius = 4;

        float beatsPerPixel = _cellBeats / _cellWidth;
        float leftPosition = _left * beatsPerPixel;
        float rightPosition = leftPosition + (width() * beatsPerPixel);

        int fullHeight = height()/2 - 8;

        std::sort(_items->begin(), _items->end(), [](const Track::PitchChange* a, const Track::PitchChange* b) {
            return a->time() < b->time();
        });

        QList<QPair<const Track::PitchChange*, QRect>> rects;
        const Track::PitchChange* onLeft = nullptr;
        float onLeftDiff = 100;
        const Track::PitchChange* onRight = nullptr;
        float onRightDiff = 100;
        for (const Track::PitchChange* item : *_items) {
            if (item->time() < leftPosition) {
                float leftDiff = leftPosition - item->time();
                if (std::abs(leftDiff) < onLeftDiff) {
                    onLeftDiff = std::abs(leftDiff);
                    onLeft = item;
                }
            }
            if (leftPosition <= item->time() && item->time() <= rightPosition) {
                int positionPixel = (item->time() - leftPosition) / beatsPerPixel;
                QPoint topLeft = QPoint(positionPixel - pointRadius, height()/2 - fullHeight * item->pitch() - pointRadius);
                rects.append(QPair<const Track::PitchChange*, QRect>(item, QRect(topLeft, topLeft + QPoint(pointRadius * 2, pointRadius * 2))));
            }
            if (item->time() > rightPosition) {
                float rightDiff = item->time() - rightPosition;
                if (std::abs(rightDiff) < onRightDiff) {
                    onRightDiff = std::abs(rightDiff);
                    onRight = item;
                }
            }
        }

        const QRect* lastRect = nullptr;
        for (const auto& rect : rects) {
            if (lastRect) {
                painter.drawLine(lastRect->center(), QPoint(rect.second.center().x(), lastRect->center().y()));
            }
            lastRect = &rect.second;
        }

        if (onLeft) {
            int onLeftPositionPixel = (onLeft->time() - leftPosition) / beatsPerPixel;
            QPoint leftTopLeft = QPoint(onLeftPositionPixel - pointRadius, height()/2 - fullHeight * onLeft->pitch() - pointRadius);
            QRect onLeftRect(leftTopLeft, leftTopLeft + QPoint(pointRadius * 2, pointRadius * 2));

            if (!rects.empty()) {
                painter.drawLine(onLeftRect.center(), QPoint(rects.first().second.center().x(), onLeftRect.center().y()));
            } else if (onRight) {
                int onRightPositionPixel = (onRight->time() - leftPosition) / beatsPerPixel;
                QPoint rightTopLeft = QPoint(onRightPositionPixel - pointRadius, height()/2 - fullHeight * onRight->pitch() - pointRadius);
                QRect onRightRect(rightTopLeft, rightTopLeft + QPoint(pointRadius * 2, pointRadius * 2));

                painter.drawLine(onLeftRect.center(), QPoint(onRightRect.center().x(), onLeftRect.center().y()));
            } else {
                painter.drawLine(onLeftRect.center(), QPoint(width(), onLeftRect.center().y()));
            }
        }


        if (onRight) {
            int onRightPositionPixel = (onRight->time() - leftPosition) / beatsPerPixel;
            QPoint rightTopLeft = QPoint(onRightPositionPixel - pointRadius, height()/2 - fullHeight * onRight->pitch() - pointRadius);
            QRect onRightRect(rightTopLeft, rightTopLeft + QPoint(pointRadius * 2, pointRadius * 2));

            if (!rects.empty()) {
                painter.drawLine(rects.last().second.center(), QPoint(onRightRect.center().x(), rects.last().second.center().y()));
            }
        } else if (!rects.empty()) {
            painter.drawLine(rects.last().second.center(), QPoint(width(), rects.last().second.center().y()));
        }

        for (const auto& rect : rects) {
            QColor color = _selectedItems.contains(rect.first) ? _areaSelectionColor : _color;
            painter.setBrush(color);
            painter.setPen(color.darker());
            painter.drawEllipse(rect.second);
        }
    }

    if (_selecting) {
        QRect rect(_fromPoint, QSize(_toPoint.x() - _fromPoint.x(), _toPoint.y() - _fromPoint.y()));
        QColor selectionColorWithAlpha = _areaSelectionColor;
        selectionColorWithAlpha.setAlpha(128);
        painter.setPen(selectionColorWithAlpha.darker());
        painter.setBrush(selectionColorWithAlpha);

        painter.drawRect(rect);
    }
}

void PianoRollPitchWidget::paintPartial(QPaintEvent *event)
{

}

float PianoRollPitchWidget::getScrollPercentage()
{
    int scrollWidth = qMax(0, length() - width());

    if (scrollWidth == 0) return 0;

    return (float)_left / (float)scrollWidth;
}

void PianoRollPitchWidget::setScrollPercentage(const float percent)
{
    int scrollWidth = qMax(0, length() - width());
    _left = percent * scrollWidth;

    setNeedsFullPaint();
    update();
}

void PianoRollPitchWidget::scrollBy(const int pixels)
{
    _left += pixels;
    setNeedsFullPaint();
    update();
}

int PianoRollPitchWidget::length() const
{
    float end = 0;
    if (_items) {
        for (const Track::PitchChange* item : *_items) {
            if (item->time() > end) {
                end = item->time();
            }
        }
    }

    int visibleLength = ((int)(end / _app->project().beatsPerBar()) + 1) * _app->project().beatsPerBar();
    int totalWidth = _cellWidth * visibleLength / _cellBeats;

    return totalWidth;
}

void PianoRollPitchWidget::setItems(QList<Track::PitchChange *> *items, const int pitchRange)
{
    _items = items;
    _pitchRange = pitchRange;
}

void PianoRollPitchWidget::setCellWidth(const int width)
{
    _cellWidth = width;

    setNeedsFullPaint();
    update();
}

void PianoRollPitchWidget::setCellBeats(const float beats)
{
    _cellBeats = beats;
}

const QList<Track::PitchChange *> &PianoRollPitchWidget::selectedItems()
{
    return _selectedItems;
}

const QColor &PianoRollPitchWidget::backgroundColor() const
{
    return _backgroundColor;
}

const QColor &PianoRollPitchWidget::borderColor() const
{
    return _borderColor;
}

const QColor &PianoRollPitchWidget::itemColor() const
{
    return _color;
}

const QColor &PianoRollPitchWidget::areaSelectionColor() const
{
    return _areaSelectionColor;
}

void PianoRollPitchWidget::setBackgroundColor(const QColor &color)
{
    _backgroundColor = color;
}

void PianoRollPitchWidget::setBorderColor(const QColor &color)
{
    _borderColor = color;
}

void PianoRollPitchWidget::setItemColor(const QColor &color)
{
    _color = color;
}

void PianoRollPitchWidget::setAreaSelectionColor(const QColor &color)
{
    _areaSelectionColor = color;
}

void PianoRollPitchWidget::mousePressEvent(QMouseEvent *event)
{
    _fromPoint = event->pos();
}

void PianoRollPitchWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (_selecting) {
        _selecting = false;

        float fromPitch = 1.0f - 2 * (float)_fromPoint.y() / height();
        float toPitch = 1.0f - 2 * (float)_toPoint.y() / height();

        if (fromPitch > toPitch) {
            float temp = fromPitch;
            fromPitch = toPitch;
            toPitch = temp;
        }

        float beatsPerPixel = _cellBeats / _cellWidth;
        float leftPosition = _left * beatsPerPixel;
        float fromPosition = leftPosition + (_fromPoint.x() * beatsPerPixel);
        float toPosition = leftPosition + (_toPoint.x() * beatsPerPixel);

        if (fromPosition > toPosition) {
            float a = fromPosition;
            fromPosition = toPosition;
            toPosition = a;
        }

        QList<Track::PitchChange*> selectedItems = *_items;
        selectedItems.erase(std::remove_if(selectedItems.begin(), selectedItems.end(), [=](Track::PitchChange* item) {
                                return (item->pitch() < fromPitch || item->pitch() > toPitch) ||
                                       (item->time() < fromPosition || item->time() >= toPosition);
                            }), selectedItems.end());

        if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
            std::for_each(selectedItems.begin(), selectedItems.end(), [&](Track::PitchChange* item) {
                if (!_selectedItems.contains(item)) {
                    _selectedItems.append(item);
                }
            });
        } else {
            _selectedItems = selectedItems;
        }
    } else {
        float pitch = 1.0f - 2 * (float)event->pos().y() / height();

        float beatsPerPixel = _cellBeats / _cellWidth;
        float leftPosition = _left * beatsPerPixel;
        float mousePosition = leftPosition + (event->pos().x() * beatsPerPixel);
        float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;


        if (event->button() == Qt::LeftButton) {
            if (_itemUnderCursor) {
                if (!_selectedItems.contains(_itemUnderCursor)) {
                    _selectedItems = QList<Track::PitchChange*>({_itemUnderCursor});
                    setNeedsFullPaint();
                    update();
                }
                return;
            } else {
                bool stop = !_selectedItems.empty();
                _selectedItems.clear();
                setNeedsFullPaint();
                update();
                if (stop) return;
            }

            QList<Track::PitchChange*>::Iterator it;
            if ((it = std::find_if(_items->begin(), _items->end(), [=,this](Track::PitchChange* const pc) {
                    return pc->time() == (_snap ? mousePositionSnapped : mousePosition);
                })) == _items->end()) {
                emit pitchChangeAdded(_snap ? mousePositionSnapped : mousePosition, pitch);
            } else {
                _app->undoStack().push(new EditPitchChangeCommand(_app->window(), *it, (*it)->time(), pitch));
            }
        } else {
            emit pitchChangeRemoved(_itemUnderCursor);
        }
    }

    setNeedsFullPaint();
    update();
}

void PianoRollPitchWidget::mouseMoveEvent(QMouseEvent *event)
{

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float mousePosition = qMax(leftPosition + (event->pos().x() * beatsPerPixel), 0.0f);
    float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;

    _mousePosition = (_snap ? mousePositionSnapped : mousePosition);

    if (!_items)
        return;

    float pitch = 1.0f - 2 * (float)event->pos().y() / height();

    float time;
    float newPitch;

    float timeDelta;
    float pitchDelta;
    if (event->buttons() & Qt::LeftButton) {
        if (_itemUnderCursor && !_selectedItems.contains(_itemUnderCursor)) {
            _selectedItems = QList({ _itemUnderCursor });
        }

        if (_selectedItems.contains(_itemUnderCursor)) {
            timeDelta = (_snap ? mousePositionSnapped : mousePosition) - _itemUnderCursor->time();
            pitchDelta = pitch - _itemUnderCursor->pitch();

            for (Track::PitchChange* item : _selectedItems) {
                time = item->time();
                newPitch = item->pitch();
                time = time + timeDelta;
                newPitch = newPitch + pitchDelta;

                _app->undoStack().push(new EditPitchChangeCommand(_app->window(), item, time, newPitch, _selectedItems));
                setNeedsFullPaint();
                update();
            }
        } else {
            _selecting = true;
            _toPoint = event->pos();
            setNeedsFullPaint();
            update();
        }
    } else {
        _itemUnderCursor = nullptr;
        for (Track::PitchChange* item : *_items) {
            if (qAbs(mousePosition - (item->time())) < (8 * beatsPerPixel) &&
                qAbs(pitch - (item->pitch())) < (8 * (2.0f/height()))) {
                _itemUnderCursor = item;
                break;
            }
        }

        if (_itemUnderCursor) {
            setCursor(Qt::SizeAllCursor);
        } else {
            unsetCursor();
        }
    }
}

void PianoRollPitchWidget::setSnap(const bool enabled)
{
    _snap = enabled;
}
