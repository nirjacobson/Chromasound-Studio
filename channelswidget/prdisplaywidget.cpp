#include "prdisplaywidget.h"

constexpr QColor PRDisplayWidget::BorderColor;
constexpr QColor PRDisplayWidget::BackgroundColor;

PRDisplayWidget::PRDisplayWidget(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(QSize(192, 64));
}

void PRDisplayWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(BackgroundColor, Qt::SolidPattern));
    painter.setPen(QPen(BorderColor));
    painter.drawRect(rect());
}
