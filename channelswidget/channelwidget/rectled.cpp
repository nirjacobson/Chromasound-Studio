#include "rectled.h"

RectLED::RectLED(QWidget *parent, const QColor& color, const QColor& selectedColor)
    : QWidget{parent}
    , _color(color)
    , _selectedColor(selectedColor)
    , _selected(false)
    , _onFunction([](){ return false; })
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

void RectLED::setOnFunction(std::function<bool ()> func)
{
    _onFunction = func;
}

void RectLED::paintEvent(QPaintEvent*)
{
    bool on = _onFunction();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor color = on ? _selected ? _selectedColor : _color : _color.darker();
    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.setPen(QPen(_selected ? _selectedColor : color.darker(), _selected ? 2 : 1));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));
}

void RectLED::mousePressEvent(QMouseEvent*)
{
    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        setSelected(!selected());
    } else {
        setSelected(true);
        emit clicked();
    }
}

void RectLED::mouseDoubleClickEvent(QMouseEvent*)
{
    if (Qt::ShiftModifier != QApplication::keyboardModifiers()) {
        emit doubleClicked();
    }
}

const QColor& RectLED::color() const
{
    return _color;
}

void RectLED::setColor(const QColor& color)
{
    _color = color;
}

const QColor& RectLED::selectedColor() const
{
    return _selectedColor;
}

void RectLED::setSelectedColor(const QColor& color)
{
    _selectedColor = color;
}
