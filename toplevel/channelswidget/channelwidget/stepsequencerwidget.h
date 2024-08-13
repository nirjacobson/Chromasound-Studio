#ifndef STEPSEQUENCERWIDGET_H
#define STEPSEQUENCERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

#include "application.h"
#include "commands/addnotecommand.h"
#include "commands/removenotecommand.h"
#include "common/damagewidget/damagewidget.h"

class StepSequencerWidget : public DamageWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor stepColor READ stepColor WRITE setStepColor)
        Q_PROPERTY(QColor otherStepColor READ otherStepColor WRITE setOtherStepColor)
        Q_PROPERTY(QColor activeStepLightColor READ activeStepLightColor WRITE setActiveStepLightColor)
        Q_PROPERTY(int stepRadius READ stepRadius WRITE setStepRadius)

    public:
        explicit StepSequencerWidget(QWidget *parent = nullptr, Application* app = nullptr, int index = 0);
        ~StepSequencerWidget();

        static constexpr int StepWidth = 16;
        static constexpr int StepSpacing = 6;

        void setApplication(Application* app);
        void setIndex(const int i);

        void doUpdate(const float position, const bool full);

    protected:
        void mousePressEvent(QMouseEvent* event);

    private:
        Application* _app;
        float _appPosition;

        int _index;

        QColor _stepColor;
        QColor _otherStepColor;
        QColor _activeStepLightColor;
        int _stepRadius;

        bool* _steps;

        const QColor& stepColor() const;
        const QColor& otherStepColor() const;
        const QColor& activeStepLightColor() const;
        int stepRadius() const;

        void setStepColor(const QColor& color);
        void setOtherStepColor(const QColor& color);
        void setActiveStepLightColor(const QColor& color);
        void setStepRadius(const int radius);

        // QWidget interface
    protected:
        void resizeEvent(QResizeEvent*);

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);
};

#endif // STEPSEQUENCERWIDGET_H
