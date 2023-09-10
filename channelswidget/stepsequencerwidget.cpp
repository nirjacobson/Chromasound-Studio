#include "stepsequencerwidget.h"

constexpr QColor StepSequencerWidget::StepColor;
constexpr QColor StepSequencerWidget::AltStepColor;
constexpr QColor StepSequencerWidget::ActiveStepLightColor;

StepSequencerWidget::StepSequencerWidget(QWidget *parent, Application* app, int index)
    : QWidget(parent)
    , _app(app)
    , _index(index)
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

void StepSequencerWidget::paintEvent(QPaintEvent*)
{
    Pattern& pattern = _app->project().getFrontPattern();

    float beatsPerStep = 0.25;

    int numSteps = _app->project().beatsPerBar() * 4;
    QSize size((numSteps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, height());
    setMinimumSize(size);
    setMaximumSize(size);

    bool steps[numSteps];
    if (pattern.hasTrack(_index)) {
        Track& track = pattern.getTrack(_index);

        for (int i = 0; i < numSteps; i++) {
            steps[i] = std::find_if(track.items().begin(), track.items().end(), [=](Track::Item* const item){ return item->time() == beatsPerStep * i; }) != track.items().end();
        }
    } else {
        for (int i = 0; i < numSteps; i++) {
            steps[i] = false;
        }
    }

    int step = -1;
    if (_app->playMode() == Application::PlayMode::Pattern) {
        step = _app->position() / beatsPerStep;
    } else {
        QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_app->position());
        int frontPatternIdx = _app->project().frontPattern();
        if (activePatterns.contains(frontPatternIdx)) {
            step = (_app->position() - activePatterns[frontPatternIdx]) / beatsPerStep;
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for (int i = 0; i < numSteps; i++) {
        bool isAlt = (i % 8) >= 4;

        QColor stepColor = isAlt ? steps[i] ? AltStepColor : AltStepColor.darker() : steps[i] ? StepColor : StepColor.darker();
        QColor borderColor = stepColor.darker();

        painter.setBrush(QBrush(stepColor, Qt::SolidPattern));
        painter.setPen(QPen(borderColor));

        QPoint stepTopLeft = QPoint(i * (StepWidth + StepSpacing) + 1, 1);
        QPoint stepBottomRight = stepTopLeft + QPoint(StepWidth, rect().height() - 3);
        QRect stepRect(stepTopLeft, stepBottomRight);

        QPainterPath path;
        path.addRoundedRect(stepRect, StepRadius, StepRadius);
        painter.fillPath(path, painter.brush());
        painter.drawPath(path);

        if (_app->isPlaying() && _app->project().getChannel(_index).enabled() && steps[i] && step >= 0 && i == step) {
            painter.setPen(QPen(ActiveStepLightColor, 2));
        } else {
            painter.setPen(QPen(borderColor, 2));
        }
        QPoint p1 = stepTopLeft + QPoint(4, 8);
        QPoint p2 = p1 + QPoint(StepWidth - 8, 0);
        painter.drawLine(p1, p2);
    }
}

void StepSequencerWidget::mousePressEvent(QMouseEvent* event)
{
    Pattern& pattern = _app->project().getFrontPattern();
    Track& track = pattern.hasTrack(_index) ? pattern.getTrack(_index) : pattern.addTrack(_index);

    float beatsPerStep = 0.25;

    int step = event->x() / (StepWidth + StepSpacing);
    float time = step * beatsPerStep;
    if (event->button() == Qt::LeftButton) {
        track.addItem(time, Note(12 * 5, beatsPerStep));
    } else {
        track.removeItem(time);
    }

    update();
}

void StepSequencerWidget::resizeEvent(QResizeEvent*)
{
    int numSteps = _app->project().beatsPerBar() * 4;
    setMinimumSize(QSize((numSteps-1) * (StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing) + StepSequencerWidget::StepWidth + 3, 8));
}
