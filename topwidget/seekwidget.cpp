#include "seekwidget.h"

SeekWidget::SeekWidget(QWidget* parent, Application* app)
    : QSlider(parent)
    , _app(app)
{

}

void SeekWidget::setApplication(Application* app)
{
    _app = app;
}

void SeekWidget::paintEvent(QPaintEvent* event)
{   
    if (_app->playMode() == Application::Pat) {
        float minPatternLength = 4;
        int patternLength = qMax(minPatternLength, _app->project().getPattern(_app->activePattern()).getLength());
        float percentage = _app->position() / (float)patternLength;

        setValue(percentage * maximum());
    } else {
        int songLength = _app->project().getLength();
        float percentage = _app->position() / (float)songLength;

        setValue(percentage * maximum());
    }
    QSlider::paintEvent(event);
}
