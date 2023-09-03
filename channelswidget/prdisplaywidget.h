#ifndef PRDISPLAYWIDGET_H
#define PRDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>

#include "application.h"

class PRDisplayWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit PRDisplayWidget(QWidget *parent = nullptr, Application* app = nullptr, int index = 0);

        void setApplication(Application* app);
        void setIndex(const int i);

    signals:

    protected:
        void paintEvent(QPaintEvent* event);

    private:
        static constexpr QColor BorderColor = QColor(128,128,128);
        static constexpr QColor BackgroundColor = QColor(255,255,255);
        static constexpr QColor CursorColor = QColor(64, 192, 64);
        static constexpr QColor ItemColor = QColor(128, 128, 255);

        Application* _app;

        int _index;

        QPair<int, int> range() const;
};

#endif // PRDISPLAYWIDGET_H
