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

    private:
        Application* _app;

    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // SEEKWIDGET_H
