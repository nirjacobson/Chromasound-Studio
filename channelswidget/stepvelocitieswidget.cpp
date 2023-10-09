#include "stepvelocitieswidget.h"

constexpr QColor StepVelocitiesWidget::BarColor;

StepVelocitiesWidget::StepVelocitiesWidget(QWidget* parent, Application* app)
    : QWidget{parent}
    , _app(app)
    , _index(0)
{

}

void StepVelocitiesWidget::setApplication(Application* app)
{
    _app = app;
}

void StepVelocitiesWidget::setChannel(const int idx)
{
    _index = idx;
}

void StepVelocitiesWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    Pattern& pattern = _app->project().getFrontPattern();

    float beatsPerStep = 0.25;

    int numSteps = _app->project().beatsPerBar() * 4;

    int width = StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing;

    int steps[numSteps];
    if (pattern.hasTrack(_index)) {
        Track& track = pattern.getTrack(_index);

        for (int i = 0; i < numSteps; i++) {
            auto it = std::find_if(track.items().begin(), track.items().end(), [=](Track::Item* const item){ return item->time() == beatsPerStep * i; });
            if (it != track.items().end()) {
                steps[i] = (*it)->note().velocity();
            } else {
                steps[i] = -1;
            }
        }
    } else {
        for (int i = 0; i < numSteps; i++) {
            steps[i] = -1;
        }
    }

    int fullBarHeight = height() - 16;


    if (pattern.hasTrack(_index)) {
        painter.setPen(BarColor.darker());
        painter.setBrush(BarColor);
        for (int x = 0; x < numSteps; x++) {
            if (steps[x] < 0)
                continue;

            QPoint topLeft = QPoint(x * width, height() - fullBarHeight * (float)steps[x]/float(100));
            QRect rect(topLeft, topLeft + QPoint(width, height() - topLeft.y()));
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);
        }
    }
}

void StepVelocitiesWidget::mousePressEvent(QMouseEvent* event)
{
    float beatsPerStep = 0.25;
    int fullBarHeight = height() - 16;
    Pattern& pattern = _app->project().getFrontPattern();

    int width = StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing;
    int stepClicked = event->position().x() / width;

    int invertY = height() - event->position().y();
    int velocityClicked = qMin(100, (int)((float)invertY/(float)fullBarHeight * 100));

    if (pattern.hasTrack(_index)) {
        Track& track = pattern.getTrack(_index);

        auto it = std::find_if(track.items().begin(), track.items().end(), [=](Track::Item* const item){ return item->time() == beatsPerStep * stepClicked; });
        if (it != track.items().end()) {
            emit clicked(stepClicked, velocityClicked);
        }
    }
}
