#include "stepcursorwidget.h"

StepCursorWidget::StepCursorWidget(QWidget *parent, Application* app)
    : QWidget{parent}
    , _app(app)
{

}

void StepCursorWidget::paintEvent(QPaintEvent*)
{

    int steps = _app->project().beatsPerBar() * 4;
    setMinimumSize(QSize((steps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, 8));

    float beatsPerStep = 0.25;

    int step;
    if (_app->playMode() == Application::PlayMode::Pat) {
        step = _app->position() / beatsPerStep;
    } else {
        QMap<int, float> activePatterns = _app->project().activePatternsAtTime(_app->position());

        if (!activePatterns.contains(_app->activePattern())) {
            return;
        }

        step = (_app->position() - activePatterns[_app->activePattern()]) / beatsPerStep;
    }


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(QColor(Qt::darkGray), Qt::SolidPattern));
    painter.setPen(QPen(QColor(Qt::darkGray)));

    QPoint stepTopLeft = QPoint(step * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + 1, 1);
    QPoint stepBottomRight = stepTopLeft + QPoint(StepSequencerWidget::StepWidth, rect().height() - 3);
    QRect stepRect(stepTopLeft, stepBottomRight);

    painter.drawRect(stepRect);
}

void StepCursorWidget::resizeEvent(QResizeEvent*)
{
    int numSteps = _app->project().beatsPerBar() * 4;
    setMinimumSize(QSize((numSteps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, 8));
}
