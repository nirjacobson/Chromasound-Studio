#include "gantteditorwidget.h"

GanttEditorWidget::GanttEditorWidget(QWidget *parent)
    : DamageWidget{parent}
    , _top(0)
    , _left(0)
    , _loopStart(-1)
    , _loopEnd(-1)
    , _rows(0)
    , _rowHeight(16)
    , _cellWidth(16)
    , _cellBeats(1)
    , _snap(true)
    , _invertRows(false)
    , _items(nullptr)
    , _itemUnderCursor(nullptr)
    , _itemsResizable(false)
    , _itemsMovableX(false)
    , _itemsMovableY(false)
    , _markers(nullptr)
    , _positionFunction([]() {
    return -1;
})
, _backgroundColor(Qt::white)
, _borderColor(Qt::gray)
, _itemColor(QColor(128, 128, 255))
, _cursorColor(QColor(64, 192, 64))
, _areaSelectionColor(QColor(192, 192, 255))
, _selectionColor(QColor(255, 192, 0))
, _selecting(false)
, _cellMajors({ 4 })
{
    setMouseTracking(true);
}

void GanttEditorWidget::setVerticalScrollPercentage(const float percent)
{
    int totalHeight = _rowHeight * _rows;
    int scrollHeight = qMax(0, totalHeight - height());
    _top = percent * scrollHeight;

    setNeedsFullPaint();
    update();
}

void GanttEditorWidget::setHorizontalScrollPercentage(const float percent)
{
    int totalWidth = _cellWidth * visibleLength() / _cellBeats;
    int scrollWidth = qMax(0, totalWidth - width());
    _left = percent * scrollWidth;

    setNeedsFullPaint();
    update();
}

void GanttEditorWidget::setMarkers(QMap<float, QList<GanttMarker*> >* markers)
{
    _markers = markers;
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

    setNeedsFullPaint();
    update();
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

void GanttEditorWidget::paintFull(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QBrush(_backgroundColor));

    int firstRow = _top / _rowHeight;
    int firstRowStart = firstRow * _rowHeight - _top;
    int numRows = qCeil((float)height()/_rowHeight) + 1;

    QPoint topLeft(0, firstRowStart);

    for (int i = 0; i < numRows; i++) {
        QPoint thisBottomLeft = topLeft + QPoint(0, i * _rowHeight);
        QPoint thisBottomRight = thisBottomLeft + QPoint(width(), 0);

        painter.setPen(_borderColor.lighter(120));
        for (int j = _rowMajors.size() - 1; j >= 0 ; j--) {
            int r = firstRow + i;
            if (_invertRows) r = _rows - 1 - r;

            if (r % _rowMajors[j] == (_rowMajors[j] - 1)) {
                painter.setPen(QPen(_borderColor.lighter(120), pow(2, j+1)));
                break;
            }
        }

        painter.drawLine(thisBottomLeft, thisBottomRight);
    }

    int firstCell = _left / _cellWidth;
    int firstCellStart = firstCell * _cellWidth - _left;
    int numCells = qCeil((float)width()/_cellWidth);

    topLeft = QPoint(firstCellStart, 0);

    for (int i = 0; i < numCells; i++) {
        QPoint thisTopRight = topLeft + QPoint((i + 1) * _cellWidth, 0);
        QPoint thisBottomRight = thisTopRight + QPoint(0, height());

        painter.setPen(_borderColor.lighter(120));
        for (int j = _cellMajors.size() - 1; j >= 0 ; j--) {
            if ((firstCell + i) % _cellMajors[j] == (_cellMajors[j] - 1)) {
                painter.setPen(QPen(_borderColor, pow(2, j+1)));
                break;
            }
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
            QColor color = _selectedItems.contains(item) ? _areaSelectionColor : _itemColor;

            painter.setPen(color.darker());
            painter.setBrush(color.darker(150));
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
            painter.setPen(Qt::NoPen);
            painter.setBrush(color.lighter(125));
            painter.drawRect(rect.adjusted(3, 1, -1, -3));
            painter.setBrush(color);
            painter.drawRect(rect.adjusted(3, 3, -3, -3));
        }
    }

    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);

    if (_markers) {
        for (auto it = _markers->begin(); it != _markers->end(); it++) {
            for (int i = 0; i < it->size(); i++) {
                GanttMarker* marker = (*it)[i];

                if (leftPosition <= marker->time() && marker->time() <= rightPosition) {
                    int markerPixel = ((marker->time() - leftPosition) / beatsPerPixel) + i;
                    painter.setPen(marker->color());
                    painter.drawLine(QLine(QPoint(markerPixel, 0), QPoint(markerPixel, height())));
                }
            }
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

    if (_loopStart >= 0 && _loopEnd >=0) {
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

void GanttEditorWidget::paintPartial(QPaintEvent* event)
{
    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float rightPosition = leftPosition + (width() * beatsPerPixel);
    float appPosition = _positionFunction();

    if (leftPosition <= appPosition && appPosition <= rightPosition) {
        int appPositionPixel = (appPosition - leftPosition) / beatsPerPixel;

        QPoint p1(appPositionPixel, 0);
        QPoint p2(appPositionPixel, height());

        QPainter painter(this);

        painter.setPen(_cursorColor);
        painter.drawLine(p1, p2);
    }
}

void GanttEditorWidget::showEvent(QShowEvent* event)
{
    setNeedsFullPaint();
    QWidget::showEvent(event);
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

void GanttEditorWidget::mousePressEvent(QMouseEvent* event)
{
    _fromPoint = event->pos();
}

void GanttEditorWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (_selecting) {
        _selecting = false;

        int firstRow = _top / _rowHeight;
        int firstRowStart = firstRow * _rowHeight - _top;
        int fromRow = firstRow + ((_fromPoint.y() - firstRowStart) / _rowHeight);
        int toRow = firstRow + ((_toPoint.y() - firstRowStart) / _rowHeight);

        if (_invertRows) {
            fromRow = _rows - 1 - fromRow;
            toRow = _rows - 1 - toRow;
        }

        if (fromRow > toRow) {
            int a = fromRow;
            fromRow = toRow;
            toRow = a;
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

        QList<GanttItem*> selectedItems = *_items;
        selectedItems.erase(std::remove_if(selectedItems.begin(), selectedItems.end(), [=](GanttItem* item) {
            return (item->row() < fromRow || item->row() > toRow) ||
                   ((item->time() + item->duration()) <= fromPosition || item->time() > toPosition);
        }), selectedItems.end());

        if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
            std::for_each(selectedItems.begin(), selectedItems.end(), [&](GanttItem* item) {
                if (!_selectedItems.contains(item)) {
                    _selectedItems.append(item);
                }
            });
        } else {
            _selectedItems = selectedItems;
        }
    } else {
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
            _selectedItems.clear();
            if (_itemUnderCursor) {
                if (!_selectedItems.contains(_itemUnderCursor)) {
                    _selectedItems = QList<GanttItem*>({_itemUnderCursor});
                    update();
                }

                emit itemReleased(_itemUnderCursor);

                return;
            }
            emit clicked(Qt::LeftButton, row, _snap ? mousePositionSnapped : mousePosition);
        } else {
            if (_itemUnderCursor && Qt::ShiftModifier == QApplication::keyboardModifiers()) {
                emit contextMenuRequested(_itemUnderCursor, mapToGlobal(event->pos()));
                return;
            }
            emit clicked(Qt::RightButton, row, _snap ? mousePositionSnapped : mousePosition);
        }

        _selectedItems.removeAll(_itemUnderCursor);
        _itemUnderCursor = nullptr;
    }

    setNeedsFullPaint();
    update();
}

void GanttEditorWidget::mouseMoveEvent(QMouseEvent* event)
{
    float beatsPerPixel = _cellBeats / _cellWidth;
    float leftPosition = _left * beatsPerPixel;
    float mousePosition = qMax(leftPosition + (event->pos().x() * beatsPerPixel), 0.0f);
    float mousePositionSnapped = (int)(mousePosition / _cellBeats) * _cellBeats;

    _mousePosition = (_snap ? mousePositionSnapped : mousePosition);

    if (!_items)
        return;

    int firstRow = _top / _rowHeight;
    int firstRowStart = firstRow * _rowHeight - _top;
    int row = firstRow + ((event->pos().y() - firstRowStart) / _rowHeight);

    if (_invertRows) {
        row = _rows - 1 - row;
    }

    float time;
    float end;
    int newRow;

    float timeDelta;
    int rowDelta;
    if (event->buttons() & Qt::LeftButton) {
        if (_itemUnderCursor && !_selectedItems.contains(_itemUnderCursor)) {
            _selectedItems = QList({ _itemUnderCursor });
        }

        if (_selectedItems.contains(_itemUnderCursor)) {
            switch (_cursorPositionOverItem) {
                case 0:
                    timeDelta = (_snap ? mousePositionSnapped : mousePosition) - _itemUnderCursor->time();

                    for (GanttItem* item : _selectedItems) {
                        time = item->time();
                        end = time + item->duration();
                        if (_itemsResizable) {
                            emit itemChanged(item,
                                             item->time() + timeDelta,
                                             item->row(),
                                             end - (item->time() + timeDelta));
                        }
                    }
                    break;
                case 1:
                    timeDelta = (_snap ? mousePositionSnapped : mousePosition) - _itemUnderCursor->time();
                    rowDelta = row - _itemUnderCursor->row();

                    for (GanttItem* item : _selectedItems) {
                        time = item->time();
                        newRow = item->row();
                        if (_itemsMovableX)
                            time = time + timeDelta;
                        if (_itemsMovableY)
                            newRow = newRow + rowDelta;

                        if (_itemsMovableX || _itemsMovableY) {
                            emit itemChanged(item, time, newRow, item->duration());
                        }
                    }
                    break;
                case 2:
                    timeDelta = (_snap ? mousePositionSnapped : mousePosition) - (_itemUnderCursor->time() + _itemUnderCursor->duration());

                    for (GanttItem* item : _selectedItems) {
                        if (_itemsResizable) {
                            emit itemChanged(item, item->time(), item->row(), item->duration() + timeDelta);
                        }
                    }
                    break;
            }
        } else {
            _selecting = true;
            _toPoint = event->pos();
            setNeedsFullPaint();
            update();
        }
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

    setNeedsFullPaint();
    update();
}

const QColor& GanttEditorWidget::backgroundColor() const
{
    return _backgroundColor;
}

const QColor& GanttEditorWidget::borderColor() const
{
    return _borderColor;
}

const QColor& GanttEditorWidget::itemColor() const
{
    return _itemColor;
}

const QColor& GanttEditorWidget::cursorColor() const
{
    return _cursorColor;
}

const QColor& GanttEditorWidget::areaSelectionColor() const
{
    return _areaSelectionColor;
}

const QColor& GanttEditorWidget::selectionColor() const
{
    return _selectionColor;
}

void GanttEditorWidget::setBackgroundColor(const QColor& color)
{
    _backgroundColor = color;
}

void GanttEditorWidget::setBorderColor(const QColor& color)
{
    _borderColor = color;
}

void GanttEditorWidget::setItemColor(const QColor& color)
{
    _itemColor = color;
}

void GanttEditorWidget::setCursorColor(const QColor& color)
{
    _cursorColor = color;
}

void GanttEditorWidget::setAreaSelectionColor(const QColor& color)
{
    _areaSelectionColor = color;
}

void GanttEditorWidget::setSelectionColor(const QColor& color)
{
    _selectionColor = color;
}

const QList<GanttItem*>& GanttEditorWidget::selectedItems() const
{
    return _selectedItems;
}

void GanttEditorWidget::selectItems(const QList<GanttItem*>& items)
{
    _selectedItems = items;

    setNeedsFullPaint();
    update();
}

void GanttEditorWidget::selectAllItems()
{
    selectItems(*_items);
}

void GanttEditorWidget::setCellMajors(const QList<int>& majors)
{
    _cellMajors = majors;
    std::sort(_cellMajors.begin(), _cellMajors.end());
}

void GanttEditorWidget::setRowMajors(const QList<int>& majors)
{
    _rowMajors = majors;
    std::sort(_rowMajors.begin(), _rowMajors.end());
}

float GanttEditorWidget::mousePosition() const
{
    return _mousePosition;
}

void GanttEditorWidget::setLoop(const float start, const float end)
{
    _loopStart = start;
    _loopEnd = end;

    setNeedsFullPaint();
    repaint();
    repaint();
}

