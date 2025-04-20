#include "pianorollpitchwidget.h"

PianoRollPitchWidget::PianoRollPitchWidget(QWidget *parent)
    : GanttBottomWidget(parent)
    , _left(0)
    , _cellWidth(16)
    , _cellBeats(1)
    , _color(192, 192, 255)
    , _app(nullptr)
    , _items(nullptr)
{
    setMinimumHeight(128);
    setMaximumHeight(128);
}

void PianoRollPitchWidget::setApplication(Application *app)
{
    _app = app;
}

void PianoRollPitchWidget::paintFull(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawLine(QPoint(0, height()/2), QPoint(width(), height() / 2));

    painter.setPen(_color.darker());
    painter.setBrush(_color);

    int pointRadius = 4;

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    int fullHeight = height()/2 - 8;

    if (_items) {
        std::sort(_items->begin(), _items->end(), [](const Track::PitchChange* a, const Track::PitchChange* b) {
            return a->time() < b->time();
        });

        QList<QRect> rects;
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
                rects.append(QRect(topLeft, topLeft + QPoint(pointRadius * 2, pointRadius * 2)));
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
        for (const QRect& rect : rects) {
            if (lastRect) {
                painter.drawLine(lastRect->center(), rect.center());
            }
            lastRect = &rect;
        }

        if (onLeft) {
            int onLeftPositionPixel = (onLeft->time() - leftPosition) / beatsPerPixel;
            QPoint leftTopLeft = QPoint(onLeftPositionPixel - pointRadius, height()/2 - fullHeight * onLeft->pitch() - pointRadius);
            QRect onLeftRect(leftTopLeft, leftTopLeft + QPoint(pointRadius * 2, pointRadius * 2));

            if (!rects.empty()) {
                painter.drawLine(onLeftRect.center(), rects.first().center());
            } else if (onRight) {
                int onRightPositionPixel = (onRight->time() - leftPosition) / beatsPerPixel;
                QPoint rightTopLeft = QPoint(onRightPositionPixel - pointRadius, height()/2 - fullHeight * onRight->pitch() - pointRadius);
                QRect onRightRect(rightTopLeft, rightTopLeft + QPoint(pointRadius * 2, pointRadius * 2));

                painter.drawLine(onLeftRect.center(), onRightRect.center());
            } else {
                painter.drawLine(onLeftRect.center(), QPoint(width(), onLeftRect.center().y()));
            }
        }


        if (onRight) {
            int onRightPositionPixel = (onRight->time() - leftPosition) / beatsPerPixel;
            QPoint rightTopLeft = QPoint(onRightPositionPixel - pointRadius, height()/2 - fullHeight * onRight->pitch() - pointRadius);
            QRect onRightRect(rightTopLeft, rightTopLeft + QPoint(pointRadius * 2, pointRadius * 2));

            if (!rects.empty()) {
                painter.drawLine(rects.last().center(), onRightRect.center());
            }
        } else if (!rects.empty()) {
            painter.drawLine(rects.last().center(), QPoint(width(), rects.last().center().y()));
        }

        for (const QRect& rect : rects) {
            painter.drawEllipse(rect);
        }
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

void PianoRollPitchWidget::mousePressEvent(QMouseEvent *event)
{
    int pointRadius = 4;

    int fullHeight = height()/2 - 8;
    float beatsPerPixel = _cellBeats / _cellWidth;

    float mousePosition = (_left + event->position().x()) * beatsPerPixel;
    float y = height()/2 - event->position().y();
    float pitchClicked = y / (height()/2);

    auto it = std::find_if(_items->begin(), _items->end(), [=](Track::PitchChange* const item) {
        return item->time() <= mousePosition && mousePosition <= item->time() + 2*pointRadius * beatsPerPixel;
    });

    if (it != _items->end()) {
        if (event->button() == Qt::LeftButton) {
            _app->undoStack().push(new EditPitchChangeCommand(_app->window(), *it, pitchClicked));
        } else {
            emit pitchChangeRemoved(*it);
        }
    } else {
        emit pitchChangeAdded(mousePosition, pitchClicked);
    }
}
