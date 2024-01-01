#include "stepcursorwidget.h"

StepCursorWidget::StepCursorWidget(QWidget *parent, Application* app)
    : QWidget{parent}
    , _app(app)
    , _color(QColor(Qt::darkGray))
    , _appPosition(0)
{

}

void StepCursorWidget::doUpdate(const float position)
{
    _appPosition = position;
    update();
}

void StepCursorWidget::paintEvent(QPaintEvent*)
{

    int steps = _app->project().beatsPerBar() * 4;
    setMinimumSize(QSize((steps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, 8));

    float beatsPerStep = 0.25;

    int step;
    if (_app->project().playMode() == Project::PlayMode::PATTERN) {
        step = _appPosition / beatsPerStep;
    } else {
        QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_appPosition);
        int frontPatternIdx = _app->project().frontPattern();
        if (!activePatterns.contains(frontPatternIdx)) {
            return;
        }

        step = (_appPosition - activePatterns[frontPatternIdx]) / beatsPerStep;
    }


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(_color, Qt::SolidPattern));
    painter.setPen(QPen(_color));

    QPoint stepTopLeft = QPoint(step * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + 1, 1);
    QPoint stepBottomRight = stepTopLeft + QPoint(StepSequencerWidget::StepWidth, rect().height() - 3);
    QRect stepRect(stepTopLeft, stepBottomRight);

    painter.drawRect(stepRect);
}

const QColor& StepCursorWidget::color() const
{
    return _color;
}

void StepCursorWidget::setColor(const QColor& color)
{
    _color = color;
}

void StepCursorWidget::resizeEvent(QResizeEvent*)
{
    int numSteps = _app->project().beatsPerBar() * 4;
    setMinimumSize(QSize((numSteps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, 8));
}
