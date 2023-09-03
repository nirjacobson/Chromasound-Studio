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
void PianoRollKeysWidget::setScrollPercentage(const float percent)
{
    int totalHeight = _rowHeight * _rows;
    int scrollHeight = qMax(0, totalHeight - height());
    _top = percent * scrollHeight;

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

void PianoRollKeysWidget::paintEvent(QPaintEvent* event)
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
            int offset = j > 1 ? 1 : 0;
            QPoint thisTopLeft = topLeft - QPoint(0, (offset + (j * 2)) * _rowHeight);
            QRect rect(thisTopLeft, thisTopLeft + QPoint(width()/2, _rowHeight));
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);
        }

        topLeft -= QPoint(0, octaveHeight);
    }
}
