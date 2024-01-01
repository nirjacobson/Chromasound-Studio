#ifndef STEPCURSORWIDGET_H
#define STEPCURSORWIDGET_H

#include <QWidget>
#include <QPainter>

#include "application.h"
#include "channelwidget/stepsequencerwidget.h"

class StepCursorWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor color READ color WRITE setColor)

    public:
        explicit StepCursorWidget(QWidget *parent = nullptr, Application* app = nullptr);

        void doUpdate(const float position);

    signals:

    protected:
        void paintEvent(QPaintEvent*);

    private:
        Application* _app;
        float _appPosition;

        QColor _color;

        const QColor& color() const;

        void setColor(const QColor& color);

    protected:
        void resizeEvent(QResizeEvent*);
};

#endif // STEPCURSORWIDGET_H
