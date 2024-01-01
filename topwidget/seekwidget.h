#ifndef SEEKWIDGET_H
#define SEEKWIDGET_H

#include <QSlider>

#include "application.h"

class SeekWidget : public QSlider
{
        Q_OBJECT
    public:
        SeekWidget(QWidget* parent = nullptr, Application* app = nullptr);

        void setApplication(Application* app);

        void doUpdate(const float position);

    private:
        Application* _app;
        float _appPosition;

    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // SEEKWIDGET_H
