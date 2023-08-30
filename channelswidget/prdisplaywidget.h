#ifndef PRDISPLAYWIDGET_H
#define PRDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>

class PRDisplayWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit PRDisplayWidget(QWidget *parent = nullptr);

    signals:

    protected:
        void paintEvent(QPaintEvent* event);

    private:
        static constexpr QColor BorderColor = QColor(128,128,128);
        static constexpr QColor BackgroundColor = QColor(255,255,255);
};

#endif // PRDISPLAYWIDGET_H
