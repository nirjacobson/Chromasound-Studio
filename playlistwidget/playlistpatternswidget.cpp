#include "playlistpatternswidget.h"

PlaylistPatternsWidget::PlaylistPatternsWidget(QWidget *parent)
    : GanttLeftWidget{parent}
    , _rows(0)
    , _top(0)
    , _rowHeight(16)
{
    setMinimumWidth(128);
    setMaximumWidth(128);
}

float PlaylistPatternsWidget::getScrollPercentage()
{
    int scrollHeight = qMax(0, length() - height());

    return (float)_top / (float)scrollHeight;
}

void PlaylistPatternsWidget::setScrollPercentage(const float percent)
{
    int scrollHeight = qMax(0, length() - height());
    _top = percent * scrollHeight;

    update();
}

void PlaylistPatternsWidget::scrollBy(const int pixels)
{
    _top += pixels;
    update();
}

void PlaylistPatternsWidget::setRows(int rows)
{
    _rows = rows;
}

void PlaylistPatternsWidget::setRowHeight(int height)
{
    _rowHeight = height;
}

int PlaylistPatternsWidget::length() const
{
    return _rowHeight * _rows;
}

void PlaylistPatternsWidget::paintEvent(QPaintEvent* event)
{
    int firstPattern = _top / _rowHeight;
    int firstPatternStart = firstPattern * _rowHeight - _top;

    int numPatternsAcrossHeight = qCeil((float)height()/_rowHeight);

    QPainter painter(this);
    painter.setPen(QColor(Qt::gray));
    painter.setBrush(QColor(QWidget::palette().color(QWidget::backgroundRole())));

    QPoint topLeft(0, firstPatternStart);

    for (int i = 0; i < numPatternsAcrossHeight && (firstPattern + i) < _rows; i++) {
        QPoint thisTopLeft = topLeft + QPoint(0, i * _rowHeight);
        QRect rect(thisTopLeft, thisTopLeft + QPoint(width() - 2, _rowHeight));

        painter.fillRect(rect, painter.brush());
        painter.drawRect(rect);
        painter.drawText(rect.adjusted(4, 4, 0, 0), QString("Pattern %1").arg(firstPattern + i + 1));
    }
}
