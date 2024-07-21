#include "seekwidget.h"

SeekWidget::SeekWidget(QWidget* parent, Application* app)
    : QSlider(parent)
    , _app(app)
    , _appPosition(0)
{

}

void SeekWidget::setApplication(Application* app)
{
    _app = app;
}

void SeekWidget::doUpdate(const float position)
{
    _appPosition = position;
    update();
}

void SeekWidget::paintEvent(QPaintEvent* event)
{
    if (_app->project().playMode() == Project::PlayMode::PATTERN) {
        float patternLength = _app->project().patterns().empty() ? _app->project().beatsPerBar() : _app->project().getPatternBarLength(_app->project().frontPattern());
        float percentage = _appPosition / (float)patternLength;

        setValue(percentage * maximum());
    } else {
        float songLength = qCeil(_app->project().getLength()/_app->project().beatsPerBar()) * _app->project().beatsPerBar();
        float percentage = _appPosition / (float)songLength;

        setValue(percentage * maximum());
    }
    QSlider::paintEvent(event);
}

void SeekWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked((float)event->pos().x() / (float)width());
    }
}
