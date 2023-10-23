#ifndef LED_H
#define LED_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

class LED : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor color READ color WRITE setColor)

    public:
        explicit LED(QWidget *parent = nullptr, const QColor& color = Qt::GlobalColor::green);

        void setOn(const bool on);
        bool on();

    signals:
        void clicked(bool shift);

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);

    private:
        QColor _color;
        bool _on;

        const QColor& color() const;
        void setColor(const QColor& color);
};

#endif // LED_H
