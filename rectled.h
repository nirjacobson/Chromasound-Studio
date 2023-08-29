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
        explicit RectLED(QWidget *parent = nullptr, const QColor& color = Qt::GlobalColor::lightGray, const QColor& selectedColor = Qt::GlobalColor::green);

        void setSelected(const bool selected);
        bool selected() const;

        void setOn(const bool on);
        bool on() const;

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
        bool _on;
};

#endif // RECTLED_H
