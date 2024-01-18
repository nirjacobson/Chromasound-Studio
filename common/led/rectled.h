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
        Q_PROPERTY(QColor color READ color WRITE setColor)
        Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor)

    public:
        explicit RectLED(QWidget *parent = nullptr, const QColor& color = QColor(0, 212, 0), const QColor& selectedColor = Qt::GlobalColor::cyan);

        void setSelected(const bool selected);
        bool selected() const;

        void setOnFunction(std::function<bool(void)> func);

    signals:
        void clicked();
        void doubleClicked();

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent*);
        void mouseDoubleClickEvent(QMouseEvent*);

    private:
        QColor _color;
        QColor _selectedColor;
        bool _selected;
        std::function<bool(void)> _onFunction;

        const QColor& color() const;
        void setColor(const QColor& color);

        const QColor& selectedColor() const;
        void setSelectedColor(const QColor& color);
};

#endif // RECTLED_H
