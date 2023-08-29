#include "rectled.h"

RectLED::RectLED(QWidget *parent, const QColor& color, const QColor& selectedColor)
    : QWidget{parent}
    , _color(color)
    , _selectedColor(selectedColor)
    , _selected(false)
    , _on(false)
{
    setMinimumWidth(8);
    setMaximumWidth(8);
}

void RectLED::setSelected(const bool selected)
{
    _selected = selected;
    repaint();
}

bool RectLED::selected() const
{
    return _selected;
}

void RectLED::setOn(const bool on)
{
    _on = on;
    repaint();
}

bool RectLED::on() const
{
    return _on;
}

void RectLED::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor color = _on ? _selected ? _selectedColor : _color : _color.darker();
    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.setPen(QPen(_selected ? _selectedColor : color.darker(), _selected ? 2 : 1));
    painter.drawRect(rect());
}

void RectLED::mousePressEvent(QMouseEvent* event)
{
    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        setSelected(!selected());
    } else {
        setSelected(true);
        emit clicked();
    }
}

void RectLED::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (Qt::ShiftModifier != QApplication::keyboardModifiers()) {
        emit doubleClicked();
    }
}
