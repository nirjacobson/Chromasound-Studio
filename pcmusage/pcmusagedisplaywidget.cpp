#include "pcmusagedisplaywidget.h"

PCMUsageDisplayWidget::PCMUsageDisplayWidget(QWidget *parent)
    : QWidget{parent}
    , _outlineColor(Qt::gray)
    , _items(nullptr)
{

}

void PCMUsageDisplayWidget::setItems(const QList<PCMUsageDisplayItem>* items)
{
    _items = items;
}

const QColor& PCMUsageDisplayWidget::outlineColor() const
{
    return _outlineColor;
}

void PCMUsageDisplayWidget::setOutlineColor(const QColor& color)
{
    _outlineColor = color;
}

void PCMUsageDisplayWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setPen(_outlineColor);
    painter.setBrush(_items->last().color());
    painter.drawRect(rect().adjusted(0, 0, -1, -1));

    if (!_items) return;

    int lastStart = height() - 1;
    for (int i = 0; i < _items->size() - 1; i++) {
        int height = this->height() * (float)((*_items)[i].size()) / PCMUsageDialog::MAX_SIZE;

        QRect rect(QPoint(0, (lastStart = lastStart - height)), QSize(width() - 1, height));

        painter.setPen((*_items)[i].color().darker());
        painter.setBrush((*_items)[i].color());

        painter.drawRect(rect);
    }
}
