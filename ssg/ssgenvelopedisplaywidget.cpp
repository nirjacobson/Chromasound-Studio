#include "ssgenvelopedisplaywidget.h"

QPointF SSGEnvelopeDisplayWidget::NULL_POINTF = QPointF(-1, -1);
int SSGEnvelopeDisplayWidget::PERIOD_WIDTH = 32;

SSGEnvelopeDisplayWidget::SSGEnvelopeDisplayWidget(QWidget *parent)
    : QWidget{parent}
    , _settings(nullptr)
    , _backgroundColor(Qt::white)
    , _borderColor(Qt::gray)
    , _envelopeColor(Qt::blue)
{

}

void SSGEnvelopeDisplayWidget::setSettings(const SSGEnvelopeSettings* settings)
{
    _settings = settings;
    update();
}

void SSGEnvelopeDisplayWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!_settings) {
        painter.fillRect(event->rect(), QBrush(_backgroundColor, Qt::SolidPattern));
        painter.setPen(_borderColor);
        painter.drawRect(event->rect());
        return;
    }

    painter.setPen(QPen(_envelopeColor, 2));
    painter.fillRect(event->rect(), QBrush(_backgroundColor, Qt::SolidPattern));

    const QList<QPointF> points = getPoints();

    if (points.empty()) {
        return;
    }

    int height = QWidget::height() * 0.6f;

    int offset = (QWidget::height() - height) / 2;

    QPointF lastPoint = points.first();
    for (int i = 1; i < points.size(); i++) {
        painter.drawLine(QPointF(lastPoint.x() * PERIOD_WIDTH, lastPoint.y() * height + offset),
                         QPointF(points[i].x() * PERIOD_WIDTH, points[i].y() * height + offset));
        lastPoint = points[i];
    }

    if (lastPoint.x() < (width() / (float)PERIOD_WIDTH)) {
        painter.drawLine(QPointF(lastPoint.x() * PERIOD_WIDTH, lastPoint.y() * height + offset),
                         QPointF(width(), lastPoint.y() * height + offset));
    }

    painter.setPen(_borderColor);
    painter.drawRect(event->rect());
}

QList<QPointF> SSGEnvelopeDisplayWidget::getPoints() const
{
    QList<QPointF> points;
    float start = _settings->att() ? 1 : 0;
    points.append(QPointF(0, start));
    points.append(QPointF(1, 1 - start));
    if (!_settings->cont()) {
        points.append(QPointF(1, 1));
    } else if (_settings->hold()) {
        points.append(QPointF(1, _settings->alt() ? start : 1-start));
    } else {
        for (int i = 1; i < qCeil(width() / PERIOD_WIDTH) + 1; i++) {
            if (_settings->alt()) {
                points.append(QPointF(1 + i, (i % 2 == 0) ? (1 - start) : start));
            } else {
                points.append(QPointF(i, start));
                points.append(QPointF(i + 1, 1 - start));
            }
        }
    }

    return points;
}
