#ifndef STEPVELOCITIESWIDGET_H
#define STEPVELOCITIESWIDGET_H

#include <QWidget>
#include <QPainter>

#include "application.h"
#include "stepsequencerwidget.h"

class StepVelocitiesWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit StepVelocitiesWidget(QWidget *parent = nullptr, Application* app = nullptr);

        void setApplication(Application* app);

        void setChannel(const int idx);

    signals:
        void clicked(const int step, const int velocity);

    private:
        static constexpr QColor BarColor = QColor(192, 192, 255);

        Application* _app;

        int _index;

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);
};

#endif // STEPVELOCITIESWIDGET_H
