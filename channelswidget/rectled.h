#ifndef RECTLED_H
#define RECTLED_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QtCore/qcoreevent.h>

class RectLED : public QWidget
{
        Q_OBJECT
    public:
        explicit RectLED(QWidget *parent = nullptr, const QColor& color = Qt::GlobalColor::green, const QColor& selectedColor = Qt::GlobalColor::cyan);

        void setSelected(const bool selected);
        bool selected() const;

        void setOnFunction(std::function<bool(void)> func);

    signals:
        void clicked();
        void doubleClicked();

    protected:
        void paintEvent(QPaintEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseDoubleClickEvent(QMouseEvent* event);

    private:
        QColor _color;
        QColor _selectedColor;
        bool _selected;
        std::function<bool(void)> _onFunction;
};

#endif // RECTLED_H
