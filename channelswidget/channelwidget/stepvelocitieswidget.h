#ifndef STEPVELOCITIESWIDGET_H
#define STEPVELOCITIESWIDGET_H

#include <QWidget>
#include <QPainter>

#include "application.h"
#include "stepsequencerwidget.h"

class StepVelocitiesWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor barColor READ barColor WRITE setBarColor)

    public:
        explicit StepVelocitiesWidget(QWidget *parent = nullptr, Application* app = nullptr);

        void setApplication(Application* app);

        void setChannel(const int idx);

    signals:
        void clicked(const int step, const int velocity);

    private:
        Application* _app;

        int _index;

        QColor _barColor;

        const QColor& barColor() const;

        void setBarColor(const QColor& color);

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);
};

#endif // STEPVELOCITIESWIDGET_H
