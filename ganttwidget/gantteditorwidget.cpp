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
    , _invertRows(false)
    , _items(nullptr)
    , _itemUnderCursor(nullptr)
    , _itemUnderCursorSelected(false)
    , _itemsResizable(false)
    , _itemsMovableX(false)
    , _itemsMovableY(false)
    , _positionFunction([](){ return -1; })
{
    setMouseTracking(true);
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
    int totalWidth = _cellWidth * visibleLength() / _cellBeats;
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

void GanttEditorWidget::invertRows(const bool invert)
{
    _invertRows = invert;
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

float GanttEditorWidget::length() const
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

float GanttEditorWidget::visibleLength() const
{
    return ((int)(length() / _app->project().beatsPerBar()) + 1) * _app->project().beatsPerBar();
}

void GanttEditorWidget::setItemsResizable(const bool resizable)
{
    _itemsResizable = resizable;
}

void GanttEditorWidget::setItemsMovableX(const bool movable)
{
    _itemsMovableX = movable;
}

void GanttEditorWidget::setItemsMovableY(const bool movable)
{
    _itemsMovableY = movable;
}

void GanttEditorWidget::setPositionFunction(std::function<float ()> func)
{
    _positionFunction = func;
}

void GanttEditorWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(QColor(Qt::gray));

    painter.fillRect(rect(), QBrush(QColor(Qt::white)));

    int firstRow = _top / _rowHeight;
    int firstRowStart = firstRow * _rowHeight - _top;
    int numRows = qCeil((float)height()/_rowHeight) + 1;

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

    if (_items) {
        for (const GanttItem* item : *_items) {
            int startPixelX = (float)item->time() / (float)_cellBeats * (float)_cellWidth;
            int endPixelX = startPixelX + (float)item->duration() / (float)_cellBeats * (float)_cellWidth;

            if (startPixelX > (_left + width()) || endPixelX < _left) {
                continue;
            }

            float startPixelY = (_invertRows ? (_rows - 1 - item->row()) : item->row()) * _rowHeight;
            float endPixelY = startPixelY + _rowHeight;

            if (startPixelY > (_top + height()) || endPixelY < _top) {
                continue;
            }

            QRect rect(QPoint(startPixelX - _left, startPixelY - _top), QPoint(endPixelX - _left, endPixelY - _top));

            painter.setPen(ItemColor.darker());
            painter.setBrush(ItemColor);
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
        }
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    float appPosition = _positionFunction();

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

    if (_invertRows) {
        row = _rows - 1 - row;
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float mousePosition = leftPosition + (event->pos().x() * beatsPerPixel);
    float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;


    if (event->button() == Qt::LeftButton) {
        if (_itemUnderCursor) {
            _itemUnderCursorSelected = true;
            return;
        }
        emit clicked(Qt::LeftButton, row, _snap ? mousePositionSnapped : mousePosition);
    } else {
        emit clicked(Qt::RightButton, row, mousePosition);
    }
}

void GanttEditorWidget::mouseReleaseEvent(QMouseEvent*)
{
    _itemUnderCursorSelected = false;
    _itemUnderCursor = nullptr;
}

void GanttEditorWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!_items)
        return;

    int firstRow = _top / _rowHeight;
    int firstRowStart = firstRow * _rowHeight - _top;
    int row = firstRow + ((event->pos().y() - firstRowStart) / _rowHeight);

    if (_invertRows) {
        row = _rows - 1 - row;
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float mousePosition = qMax(leftPosition + (event->pos().x() * beatsPerPixel), 0.0f);
    float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;

    float time;
    float end;
    if (_itemUnderCursorSelected) {
        switch (_cursorPositionOverItem) {
            case 0:
                time = _itemUnderCursor->time();
                end = time + _itemUnderCursor->duration();
                if (_itemsResizable) {
                    _itemUnderCursor->setTime(_snap ? mousePositionSnapped : mousePosition);
                    _itemUnderCursor->setDuration(end - (_snap ? mousePositionSnapped : mousePosition));
                    emit itemsChanged();
                }
                break;
            case 1:
                if (_itemsMovableX)
                    _itemUnderCursor->setTime(_snap ? mousePositionSnapped : mousePosition);
                if (_itemsMovableY)
                    _itemUnderCursor->setRow(row);
                if (_itemsMovableX || _itemsMovableY)
                    emit itemsChanged();
                break;
            case 2:
                if (_itemsResizable) {
                    time = _itemUnderCursor->time();
                    _itemUnderCursor->setDuration((_snap ? mousePositionSnapped : mousePosition) - time);
                    emit itemsChanged();
                }
                break;
        }

        update();
    } else {
        _itemUnderCursor = nullptr;
        float delta = beatsPerPixel * 4;
        for (GanttItem* item : *_items) {
            if (item->row() != row) {
                continue;
            }

            if (qAbs(mousePosition - (item->time())) < delta) {
                _itemUnderCursor = item;
                _cursorPositionOverItem = 0;
                break;
            } else if(qAbs(mousePosition - (item->time() + item->duration())) < delta) {
                _itemUnderCursor = item;
                _cursorPositionOverItem = 2;
                break;
            } else if (item->time() <= mousePosition && mousePosition <= (item->time() + item->duration())) {
                _itemUnderCursor = item;
                _cursorPositionOverItem = 1;
                break;
            }
        }

        if (_itemUnderCursor) {
            switch (_cursorPositionOverItem) {
                case 0:
                case 2:
                    if (_itemsResizable)
                        setCursor(Qt::SizeHorCursor);
                    break;
                case 1:
                    if (_itemsMovableX && _itemsMovableY)
                        setCursor(Qt::SizeAllCursor);
                    else if (_itemsMovableX)
                        setCursor(Qt::SizeHorCursor);
                    else if (_itemsMovableY)
                        setCursor(Qt::SizeVerCursor);
                default:
                    break;
            }
        } else {
            unsetCursor();
        }
    }
}

void GanttEditorWidget::wheelEvent(QWheelEvent* event)
{
    long oldTop = _top;
    long oldLeft = _left;
    if (event->pixelDelta().isNull()) {
        _top -= event->angleDelta().y() / 8 / 5;
        _left -= event->angleDelta().x() / 8 / 5;
    } else {
        _top -= event->pixelDelta().y();
        _left -= event->pixelDelta().x();
    }
    _top = qMin(_top, (_rows * _rowHeight) - (long)height());
    _top = qMax(_top, 0L);

    long totalWidth = _cellWidth * visibleLength() / _cellBeats;

    _left = qMin(_left, totalWidth - width());
    _left = qMax(_left, 0L);

    if (oldTop != _top) {
        emit verticalScroll(_top - oldTop);
    }

    if (oldLeft != _left) {
        emit horizontalScroll(_left - oldLeft);
    }
    update();
}
