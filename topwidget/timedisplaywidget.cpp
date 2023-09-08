#include "timedisplaywidget.h"

TimeDisplayWidget::TimeDisplayWidget(QWidget* parent, Application* app)
    : QLCDNumber(parent)
    , _app(app)
    , _mode(BS)
{

}

void TimeDisplayWidget::setApplication(Application* app)
{
    _app = app;
}

void TimeDisplayWidget::paintEvent(QPaintEvent* event)
{
    QString str;
    if (_mode == BS) {
        float beatsPerStep = 0.25;

        int steps = _app->position() / beatsPerStep;

        int stepsPerBar = _app->project().beatsPerBar() * 4;

        int bar = steps / stepsPerBar + 1;
        int step = steps % stepsPerBar + 1;
        str = QString("%1:%2").arg(bar, 2, 10, QLatin1Char('0')).arg(step, 2, 10, QLatin1Char('0'));
    } else if (_mode == MSZ) {
        float beats = _app->position();
        float seconds = (beats / _app->project().tempo()) * 60;

        str = QDateTime::fromMSecsSinceEpoch((int)(seconds * 1000)).toUTC().toString("mm:ss.zzz");
    }

    display(str);
    QLCDNumber::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QColor(Qt::GlobalColor::gray)));
    painter.drawText(4, 16, _mode == BS ? "B:S" : "M:S:Z");
}

void TimeDisplayWidget::mousePressEvent(QMouseEvent*)
{
    _mode = _mode == BS ? MSZ : BS;
    update();
}
