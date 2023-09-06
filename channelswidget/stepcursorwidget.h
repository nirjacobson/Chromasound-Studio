#ifndef STEPCURSORWIDGET_H
#define STEPCURSORWIDGET_H

#include <QWidget>
#include <QPainter>

#include "application.h"
#include "stepsequencerwidget.h"

class StepCursorWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit StepCursorWidget(QWidget *parent = nullptr, Application* app = nullptr);

    signals:

    protected:
        void paintEvent(QPaintEvent*);

    private:
        Application* _app;

    protected:
        void resizeEvent(QResizeEvent*);
};

#endif // STEPCURSORWIDGET_H
