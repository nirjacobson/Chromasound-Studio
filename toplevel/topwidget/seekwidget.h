#ifndef SEEKWIDGET_H
#define SEEKWIDGET_H

#include <QSlider>
#include <QMouseEvent>

#include "application.h"

class SeekWidget : public QSlider
{
        Q_OBJECT
    public:
        SeekWidget(QWidget* parent = nullptr, Application* app = nullptr);

        void setApplication(Application* app);

        void doUpdate(const float position);

    signals:
        void clicked(const float pos);

    private:
        Application* _app;
        float _appPosition;

    protected:
        void paintEvent(QPaintEvent* event);

        // QWidget interface
    protected:
        void mousePressEvent(QMouseEvent* event);
};

#endif // SEEKWIDGET_H
