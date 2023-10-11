#include "pianorollkeyswidget.h"

PianoRollKeysWidget::PianoRollKeysWidget(QWidget* parent)
    : GanttLeftWidget(parent)
    , _rows(0)
    , _top(0)
    , _rowHeight(16)
{
    setMinimumWidth(128);
    setMaximumWidth(128);
}

float PianoRollKeysWidget::getScrollPercentage()
{
    int scrollHeight = qMax(0, length() - height());

    return (float)_top / (float)scrollHeight;
}
void PianoRollKeysWidget::setScrollPercentage(const float percent)
{
    int scrollHeight = qMax(0, length() - height());
    _top = percent * scrollHeight;

    update();
}

void PianoRollKeysWidget::scrollBy(const int pixels)
{
    _top += pixels;
    update();
}

void PianoRollKeysWidget::setRows(int rows)
{
    _rows = rows;
}

void PianoRollKeysWidget::setRowHeight(int height)
{
    _rowHeight = height;
}

void PianoRollKeysWidget::pressKey(const int key)
{
    _onKeys.append(key);
    update();
}

void PianoRollKeysWidget::releaseKey(const int key)
{
    _onKeys.removeAll(key);
    update();
}

int PianoRollKeysWidget::length() const
{
    return _rowHeight * _rows;
}

void PianoRollKeysWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(QColor(Qt::gray));
    painter.setBrush(QColor(Qt::white));

    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;
    int bottomPositionIntoOctave = bottomPosition % octaveHeight;
    int bottomOctave = bottomPosition / octaveHeight;

    int octaveStart = height() + bottomPositionIntoOctave;
    int numOctavesAcrossHeight = qCeil((float)height()/(octaveHeight)) + 1;

    QPoint topLeft = QPoint(0, octaveStart - whiteKeyWidth);

    for (int i = 0; i < numOctavesAcrossHeight; i++) {
        for (int j = 0; j < WHITE_KEYS_PER_OCTAVE; j++) {
            int octave = bottomOctave + i;
            int key = j;
            key *= 2;
            if (key >= 6) key--;
            key = (octave * 12) + key;

            if (_onKeys.contains(key)) {
                painter.setBrush(QColor(255, 192, 192));
            } else {
                painter.setBrush(QColor(Qt::white));
            }

            QPoint thisTopLeft = topLeft - QPoint(0, (j * whiteKeyWidth));
            QRect rect(thisTopLeft, thisTopLeft + QPoint(width() - 2, whiteKeyWidth));
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);
            if (j == 0) {
                QTextOption textOption;
                textOption.setAlignment(Qt::AlignRight);
                painter.drawText(rect.adjusted(0, 8, -4, 0), QString("C%1").arg(bottomOctave + i), textOption);
            }
        }

        topLeft -= QPoint(0, octaveHeight);
    }

    painter.setPen(QColor(Qt::black));
    painter.setBrush(QColor(Qt::black));

    topLeft = QPoint(0, octaveStart - 2 * _rowHeight);

    for (int i = 0; i < numOctavesAcrossHeight; i++) {
        for (int j = 0; j < BLACK_KEYS_PER_OCTAVE; j++) {
            int octave = bottomOctave + i;
            int key = j;
            key = (key * 2) + 1;
            if (key > 3) key++;
            key = (octave * 12) + key;

            if (_onKeys.contains(key)) {
                painter.setBrush(QColor(255, 192, 192));
            } else {
                painter.setBrush(QColor(Qt::black));
            }

            int offset = j > 1 ? 1 : 0;
            QPoint thisTopLeft = topLeft - QPoint(0, (offset + (j * 2)) * _rowHeight);
            QRect rect(thisTopLeft, thisTopLeft + QPoint(width()/2, _rowHeight));
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);
        }

        topLeft -= QPoint(0, octaveHeight);
    }
}

void PianoRollKeysWidget::mousePressEvent(QMouseEvent* event)
{
    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;

    int mousePosition = height() - event->pos().y();
    int mousePositionIntoOctave = (bottomPosition + mousePosition) % octaveHeight;
    int mouseOctave = (bottomPosition + mousePosition) / octaveHeight;

    int key;
    if (event->pos().x() >= width()/2) {
        key = (mousePositionIntoOctave / whiteKeyWidth);
        key *= 2;
        if (key >= 6) {
            key--;
        }
        key = (mouseOctave * 12) + key;
    } else {
        key = (mouseOctave * 12) + (mousePositionIntoOctave / _rowHeight);
    }

    _onKeys.append(key);

    update();

    emit keyOn(key, 100);
}

void PianoRollKeysWidget::mouseReleaseEvent(QMouseEvent* event)
{
    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;

    int mousePosition = height() - event->pos().y();
    int mousePositionIntoOctave = (bottomPosition + mousePosition) % octaveHeight;
    int mouseOctave = (bottomPosition + mousePosition) / octaveHeight;

    int key;
    if (event->pos().x() >= width()/2) {
        key = (mousePositionIntoOctave / whiteKeyWidth);
        key *= 2;
        if (key >= 6) {
            key--;
        }
        key = (mouseOctave * 12) + key;
    } else {
        key = (mouseOctave * 12) + (mousePositionIntoOctave / _rowHeight);
    }

    _onKeys.removeAll(key);

    update();

    emit keyOff(key);
}
