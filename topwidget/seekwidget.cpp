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
    if (_app->playMode() == Application::Pattern) {
        float patternLength = qCeil(_app->project().getPattern(_app->activePattern()).getLength()/_app->project().beatsPerBar()) * _app->project().beatsPerBar();
        float percentage = _app->position() / (float)patternLength;

        setValue(percentage * maximum());
    } else {
        float songLength = qCeil(_app->project().getLength()/_app->project().beatsPerBar()) * _app->project().beatsPerBar();
        float percentage = _app->position() / (float)songLength;

        setValue(percentage * maximum());
    }
    QSlider::paintEvent(event);
}
