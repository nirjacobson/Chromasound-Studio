#include "led.h"

LED::LED(QWidget *parent, const QColor& color)
    : QWidget{parent}
    , _color(color)
{
    setMinimumSize(QSize(16, 16));
    setMaximumSize(QSize(16, 16));
}

void LED::setOn(const bool on)
{
    _on = on;
    repaint();
}

bool LED::on()
{
    return _on;
}

void LED::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor color = _on ? _color : _color.darker();
    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.setPen(QPen(color.darker(), 2));
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
}

void LED::mousePressEvent(QMouseEvent* event)
{
    emit clicked(Qt::ShiftModifier == QApplication::keyboardModifiers());
}
