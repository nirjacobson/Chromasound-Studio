#ifndef STEPSEQUENCERWIDGET_H
#define STEPSEQUENCERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

#include "application.h"
#include "commands/addnotecommand.h"
#include "commands/removenotecommand.h"

class StepSequencerWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit StepSequencerWidget(QWidget *parent = nullptr, Application* app = nullptr, int index = 0);

        static constexpr int StepWidth = 16;
        static constexpr int StepSpacing = 6;

        void setApplication(Application* app);
        void setIndex(const int i);

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent(QMouseEvent* event);

    private:
        static constexpr QColor StepColor = QColor(192,192,255);
        static constexpr QColor AltStepColor = QColor(255,192,192);
        static constexpr QColor ActiveStepLightColor = QColor(255, 192, 0);
        static constexpr int StepRadius = 2;

        Application* _app;

        int _index;

        // QWidget interface
    protected:
        void resizeEvent(QResizeEvent*);
};

#endif // STEPSEQUENCERWIDGET_H
