#include "stepsequencerwidget.h"

StepSequencerWidget::StepSequencerWidget(QWidget *parent, Application* app, int index)
    : QWidget(parent)
    , _app(app)
    , _index(index)
    , _stepColor(QColor(192,192,255))
    , _otherStepColor(QColor(255,192,192))
    , _activeStepLightColor(QColor(255,192,0))
    , _stepRadius(2)
    , _appPosition(0)
{

}

void StepSequencerWidget::setApplication(Application* app)
{
    _app = app;
}

void StepSequencerWidget::setIndex(const int i)
{
    _index = i;
}

void StepSequencerWidget::doUpdate(const float position)
{
    _appPosition = position;
    update();
}

void StepSequencerWidget::paintEvent(QPaintEvent*)
{
    Pattern& pattern = _app->project().getFrontPattern();

    float beatsPerStep = 0.25;

    int numSteps = _app->project().beatsPerBar() * 4;
    QSize size((numSteps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, height());
    setMinimumSize(size);
    setMaximumSize(size);

    bool* steps = new bool[numSteps];
    if (pattern.hasTrack(_index)) {
        Track& track = pattern.getTrack(_index);

        for (int i = 0; i < numSteps; i++) {
            steps[i] = std::find_if(track.items().begin(), track.items().end(), [=](Track::Item* const item) {
                return item->time() == beatsPerStep * i;
            }) != track.items().end();
        }
    } else {
        for (int i = 0; i < numSteps; i++) {
            steps[i] = false;
        }
    }

    int step = -1;
    if (_app->project().playMode() == Project::PlayMode::PATTERN) {
        step = _appPosition / beatsPerStep;
    } else {
        QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_appPosition);
        int frontPatternIdx = _app->project().frontPattern();
        if (activePatterns.contains(frontPatternIdx)) {
            step = (_appPosition - activePatterns[frontPatternIdx]) / beatsPerStep;
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for (int i = 0; i < numSteps; i++) {
        bool isAlt = (i % 8) >= 4;

        QColor stepColor = isAlt ? steps[i] ? _otherStepColor : _otherStepColor.darker() : steps[i] ? _stepColor : _stepColor.darker();
        QColor borderColor = stepColor.darker();

        painter.setBrush(QBrush(stepColor, Qt::SolidPattern));
        painter.setPen(QPen(borderColor));

        QPoint stepTopLeft = QPoint(i * (StepWidth + StepSpacing) + 1, 1);
        QPoint stepBottomRight = stepTopLeft + QPoint(StepWidth, rect().height() - 3);
        QRect stepRect(stepTopLeft, stepBottomRight);

        QPainterPath path;
        path.addRoundedRect(stepRect, _stepRadius, _stepRadius);
        painter.fillPath(path, painter.brush());
        painter.drawPath(path);

        if (_app->isPlaying() && _app->project().getChannel(_index).enabled() && steps[i] && step >= 0 && i == step) {
            painter.setPen(QPen(_activeStepLightColor, 2));
        } else {
            painter.setPen(QPen(borderColor, 2));
        }
        QPoint p1 = stepTopLeft + QPoint(4, 8);
        QPoint p2 = p1 + QPoint(StepWidth - 8, 0);
        painter.drawLine(p1, p2);
    }

    delete [] steps;
}

void StepSequencerWidget::mousePressEvent(QMouseEvent* event)
{
    Pattern& pattern = _app->project().getFrontPattern();
    Track& track = pattern.hasTrack(_index) ? pattern.getTrack(_index) : pattern.addTrack(_index);

    float beatsPerStep = 0.25;

    int step = event->position().x() / (StepWidth + StepSpacing);
    float time = step * beatsPerStep;

    if (event->button() == Qt::LeftButton) {
        track.addItem(time, Note(12 * 5, beatsPerStep));
        _app->undoStack().push(new AddNoteCommand(_app->window(), track, time, Note(12 * 5, beatsPerStep)));
    } else {
        _app->undoStack().push(new RemoveNoteCommand(_app->window(), track, time, Note(12 * 5, beatsPerStep)));
    }
}

const QColor& StepSequencerWidget::stepColor() const
{
    return _stepColor;
}

const QColor& StepSequencerWidget::otherStepColor() const
{
    return _otherStepColor;
}

const QColor& StepSequencerWidget::activeStepLightColor() const
{
    return _activeStepLightColor;
}

int StepSequencerWidget::stepRadius() const
{
    return _stepRadius;
}

void StepSequencerWidget::setStepColor(const QColor& color)
{
    _stepColor = color;
}

void StepSequencerWidget::setOtherStepColor(const QColor& color)
{
    _otherStepColor = color;
}

void StepSequencerWidget::setActiveStepLightColor(const QColor& color)
{
    _activeStepLightColor = color;
}

void StepSequencerWidget::setStepRadius(const int radius)
{
    _stepRadius = radius;
}

void StepSequencerWidget::resizeEvent(QResizeEvent*)
{
    int numSteps = _app->project().beatsPerBar() * 4;
    setMinimumSize(QSize((numSteps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, 8));
}
