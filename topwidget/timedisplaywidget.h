#ifndef TIMEDISPLAYWIDGET_H
#define TIMEDISPLAYWIDGET_H

#include <QLCDNumber>
#include <QDateTime>
#include <QPainter>
#include <QColor>

#include "application.h"

class TimeDisplayWidget : public QLCDNumber
{
        Q_OBJECT
    public:
        enum Mode {
            MSZ,
            BS
        };

        TimeDisplayWidget(QWidget* parent = nullptr, Application* app = nullptr);
        void setApplication(Application* app);

    protected:
        void paintEvent(QPaintEvent* event);
        void mousePressEvent(QMouseEvent*);

    private:
        Application* _app;

        Mode _mode;
};

#endif // TIMEDISPLAYWIDGET_H
