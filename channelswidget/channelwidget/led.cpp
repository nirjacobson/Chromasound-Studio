#include "led.h"

LED::LED(QWidget *parent, const QColor& color)
    : QWidget{parent}
    , _on(false)
    , _color(color)
{

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

void LED::paintEvent(QPaintEvent*)
{
    QRect drawRect(QPoint(), QSize(16, 16));
    drawRect.translate(rect().center() - drawRect.center());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor color = _on ? _color : _color.darker();

    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.setPen(QPen(color, 2));
    painter.drawEllipse(drawRect.adjusted(1, 1, -1, -1));

    painter.setBrush(QBrush(color.lighter(), Qt::SolidPattern));
    painter.setPen(QPen(color.lighter(), 2));
    painter.drawEllipse(drawRect.adjusted(5, 1, -1, -5));

    painter.setBrush(QBrush(Qt::NoBrush));
    painter.setPen(QPen(color.darker(), 1));
    painter.drawEllipse(drawRect.adjusted(1, 1, -1, -1));
}

void LED::mousePressEvent(QMouseEvent* event)
{
    QRect drawRect(QPoint(), QSize(16, 16));
    drawRect.translate(rect().center() - drawRect.center());

    if (drawRect.contains(event->pos())) {
        emit clicked(Qt::ShiftModifier == QApplication::keyboardModifiers());
    }
}

const QColor& LED::color() const
{
    return _color;
}

void LED::setColor(const QColor& color)
{
    _color = color;
}
