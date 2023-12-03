#include "ganttscalewidget.h"

GanttScaleWidget::GanttScaleWidget(QWidget *parent)
    : QWidget{parent}
    , _scale(1)
{

}

void GanttScaleWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        _scale = qMax(MinScale, qMin(MaxScale, 1 + ((height()/2) - event->pos().y()) * ScalePerPixel));

        emit scaleChanged(_scale);
        update();
    }
}

void GanttScaleWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit scaleChanged(_scale = 1);
    update();
}

void GanttScaleWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::gray);
    painter.setBrush(Qt::NoBrush);

    int minWidth = 4;
    int maxWidth = width() - 4;
    int width = minWidth + ((_scale/(MaxScale - MinScale)) * (maxWidth - minWidth));
    int height = GanttScaleWidget::height() - 4;

    QPoint topLeft((GanttScaleWidget::width() / 2) - (width / 2), 2);
    QRect rect(topLeft, QSize(width, height));

    painter.drawRect(rect);
}
