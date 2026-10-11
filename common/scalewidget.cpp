#include "scalewidget.h"

ScaleWidget::ScaleWidget(QWidget *parent)
    : QWidget{parent}
    , _scale(1)
{

}

void ScaleWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        _scale = qMax(MinScale, qMin(MaxScale, 1 + ((height()/2) - event->pos().y()) * ScalePerPixel));

        emit scaleChanged(_scale);
        update();
    }
}

void ScaleWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit scaleChanged(_scale = 1);
    update();
}

void ScaleWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QWidget::palette().color(QWidget::backgroundRole()).darker());
    painter.setBrush(Qt::NoBrush);

    int minWidth = 4;
    int maxWidth = width() - 4;
    int width = minWidth + ((_scale/(MaxScale - MinScale)) * (maxWidth - minWidth));
    int height = width;

    QPoint topLeft((ScaleWidget::width() / 2) - (width / 2), (ScaleWidget::height() / 2) - (height / 2));
    QRect rect(topLeft, QSize(width, height));

    painter.drawRect(rect);
}
