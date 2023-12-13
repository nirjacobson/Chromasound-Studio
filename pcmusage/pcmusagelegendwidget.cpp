#include "pcmusagelegendwidget.h"

PCMUsageLegendWidget::PCMUsageLegendWidget(QWidget *parent)
    : QWidget{parent}
    , _foregroundColor(QWidget::palette().color(QWidget::backgroundRole()).darker())
{

}

void PCMUsageLegendWidget::setItems(const QList<PCMUsageDisplayItem>* items)
{
    _items = items;
}

const QColor& PCMUsageLegendWidget::foregroundColor() const
{
    return _foregroundColor;
}

void PCMUsageLegendWidget::setForegroundColor(const QColor& color)
{
    _foregroundColor = color;
}

void PCMUsageLegendWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    int start = 0;
    QRect labelBoundingRect;
    for (int i = 0; i < _items->size(); i++) {
        QRect rect(QPoint(0, start), QSize(36, 24));

        painter.setPen((*_items)[i].color().darker());
        painter.setBrush((*_items)[i].color());

        painter.drawRect(rect);

        rect = rect.adjusted(44, 0, width() - 44, 0);

        QRect br;
        painter.setPen(_foregroundColor);
        painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, (*_items)[i].name(), &br);

        if (br.width() > labelBoundingRect.width()) {
            labelBoundingRect = br;
        }

        start += 32;
    }

    start = 0;
    for (int i = 0; i < _items->size(); i++) {
        QRect rect(QPoint(labelBoundingRect.right() + 8, start), QSize(72, 24));

        painter.setPen(_foregroundColor);
        painter.drawText(rect, Qt::AlignRight | Qt::AlignVCenter, (*_items)[i].sizeString());

        start += 32;
    }
}
