#ifndef STEPVELOCITIES_H
#define STEPVELOCITIES_H

#include <QWidget>
#include <QPainter>

#include "application.h"
#include "stepsequencerwidget.h"

class StepVelocities : public QWidget
{
        Q_OBJECT
    public:
        explicit StepVelocities(QWidget *parent = nullptr, Application* app = nullptr);

        void setChannel(const int idx);

    signals:
        void clicked(const int step, const int velocity);

    private:
        static constexpr QColor BarColor = QColor(192, 192, 255);

        Application* _app;

        int _index;

    protected:
        void paintEvent(QPaintEvent* event);
        void mousePressEvent(QMouseEvent* event);
};

#endif // STEPVELOCITIES_H
