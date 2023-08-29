#include "gantteditorwidget.h"

constexpr QColor GanttEditorWidget::CursorColor;
constexpr QColor GanttEditorWidget::ItemColor;

GanttEditorWidget::GanttEditorWidget(QWidget *parent)
    : QWidget{parent}
    , _top(0)
    , _left(0)
    , _rows(0)
    , _rowHeight(16)
    , _cellWidth(16)
    , _cellBeats(1)
    , _snap(true)
{

}

void GanttEditorWidget::setVerticalScrollPercentage(const float percent)
{
    int totalHeight = _rowHeight * _rows;
    int scrollHeight = qMax(0, totalHeight - height());
    _top = percent * scrollHeight;

    update();
}

void GanttEditorWidget::setHorizontalScrollPercentage(const float percent)
{
    int totalWidth = _cellWidth * _app->project().getLength() / _cellBeats;
    int scrollWidth = qMax(0, totalWidth - width());
    _left = percent * scrollWidth;

    update();
}

void GanttEditorWidget::setItems(QList<GanttItem*>* items)
{
    _items = items;
}

void GanttEditorWidget::setApplication(Application* app)
{
    _app = app;
}

void GanttEditorWidget::setSnap(const bool snap)
{
    _snap = snap;
}

void GanttEditorWidget::setCellWidth(int width)
{
    _cellWidth = width;
}

void GanttEditorWidget::setCellBeats(float beats)
{
    _cellBeats = beats;
}

void GanttEditorWidget::setRows(int rows)
{
    _rows = rows;
}

void GanttEditorWidget::setRowHeight(int height)
{
    _rowHeight = height;
}

void GanttEditorWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(QColor(Qt::gray));

    painter.fillRect(rect(), QBrush(QColor(Qt::white)));

    int firstRow = _top / _rowHeight;
    int firstRowStart = firstRow * _rowHeight - _top;
    int numRows = qCeil((float)height()/_rowHeight);

    QPoint topLeft(0, firstRowStart);

    for (int i = 0; i < numRows; i++) {
        QPoint thisBottomLeft = topLeft + QPoint(0, i * _rowHeight);
        QPoint thisBottomRight = thisBottomLeft + QPoint(width(), 0);

        painter.drawLine(thisBottomLeft, thisBottomRight);
    }

    int firstCell = _left / _cellWidth;
    int firstCellStart = firstCell * _cellWidth - _left;
    int numCells = qCeil((float)width()/_cellWidth);

    topLeft = QPoint(firstCellStart, 0);

    int beatsPerBar = _app->project().beatsPerBar();
    for (int i = 0; i < numCells; i++) {
        QPoint thisTopRight = topLeft + QPoint((i + 1) * _cellWidth, 0);
        QPoint thisBottomRight = thisTopRight + QPoint(0, height());
        if ((firstCell + i) % beatsPerBar == (beatsPerBar-1)) {
            painter.setPen(QPen(Qt::gray, 2));
        } else {
            painter.setPen(Qt::lightGray);
        }
        painter.drawLine(thisTopRight, thisBottomRight);
    }

    for (const GanttItem* item : *_items) {
        int startPixelX = (float)item->time() / (float)_cellBeats * (float)_cellWidth;
        int endPixelX = startPixelX + (float)item->duration() / (float)_cellBeats * (float)_cellWidth;

        if (startPixelX > (_left + width()) || endPixelX < _left) {
            continue;
        }

        float startPixelY = item->row() * _rowHeight;
        float endPixelY = startPixelY + _rowHeight;

        if (startPixelY > (_top + height()) || endPixelY < _top) {
            continue;
        }

        QRect rect(QPoint(startPixelX - _left, startPixelY - _top), QPoint(endPixelX - _left, endPixelY - _top));

        painter.setPen(ItemColor.darker());
        painter.setBrush(ItemColor);
        painter.drawRect(rect.adjusted(0, 0, -1, -1));
    }


    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    float appPosition = _app->position();

    if (leftPosition <= appPosition && appPosition <= rightPosition) {
        int appPositionPixel = (appPosition - leftPosition) / beatsPerPixel;

        QPoint p1(appPositionPixel, 0);
        QPoint p2(appPositionPixel, height());

        painter.setPen(CursorColor);
        painter.drawLine(p1, p2);
    }
}

void GanttEditorWidget::mousePressEvent(QMouseEvent* event)
{
    int firstRow = _top / _rowHeight;
    int firstRowStart = firstRow * _rowHeight - _top;
    int row = firstRow + ((event->pos().y() - firstRowStart) / _rowHeight);

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float mousePosition = leftPosition + (event->pos().x() * beatsPerPixel);
    float mousePositionSnapped = (int)mousePosition;

    if (event->button() == Qt::LeftButton) {
        emit clicked(Qt::LeftButton, row, _snap ? mousePositionSnapped : mousePosition);
    } else {
        emit clicked(Qt::RightButton, row, mousePosition);
    }
}
