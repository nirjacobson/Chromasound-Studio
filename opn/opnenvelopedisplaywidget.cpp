#include "opnenvelopedisplaywidget.h"

QPointF OPNEnvelopeDisplayWidget::NULL_POINTF = QPointF(-1, -1);

OPNEnvelopeDisplayWidget::OPNEnvelopeDisplayWidget(QWidget *parent)
    : QWidget(parent)
    , _settings(nullptr)
    , _backgroundColor(Qt::white)
    , _borderColor(Qt::gray)
    , _envelopeColor(Qt::blue)
    , _levelColor(Qt::gray)
    , _releaseColor(0, 192, 255)
{

}

void OPNEnvelopeDisplayWidget::setSettings(const OPNEnvelopeSettings* settings)
{
    _settings = settings;
    update();
}

void OPNEnvelopeDisplayWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!_settings) {
        painter.fillRect(event->rect(), QBrush(_backgroundColor, Qt::SolidPattern));
        painter.setPen(_borderColor);
        painter.drawRect(event->rect());
        return;
    }

    const QList<QPointF> points = getPoints();

    painter.fillRect(event->rect(), QBrush(_backgroundColor, Qt::SolidPattern));

    if (points[0] == NULL_POINTF) {
        painter.setPen(QPen(_envelopeColor, 2));
        painter.drawLine(0, rect().height(), rect().width(), rect().height());
    } else {
        QPoint p1(points[0].x() * rect().height(), points[0].y() * rect().height());

        painter.setPen(QPen(_levelColor, 2));
        painter.drawLine(p1, QPoint(p1.x(), rect().height()));

        painter.setPen(QPen(_envelopeColor, 2));
        painter.drawLine(QPoint(0, rect().height()), p1);

        if (points[1] == NULL_POINTF) {
            painter.setPen(QPen(_envelopeColor, 2));
            painter.drawLine(p1, QPoint(rect().width(), p1.y()));
        } else {
            QPoint p2(points[1].x() * rect().height(), points[1].y() * rect().height());

            painter.setPen(QPen(_levelColor, 2));
            painter.drawLine(p2, QPoint(p2.x(), rect().height()));

            painter.setPen(QPen(_envelopeColor, 2));
            painter.drawLine(p1, p2);

            if (points[2] == NULL_POINTF) {
                painter.setPen(QPen(_envelopeColor, 2));
                painter.drawLine(p2, QPoint(rect().width(), p2.y()));
            } else {
                painter.setPen(QPen(_envelopeColor, 2));
                QPoint p3(points[2].x() * rect().height(), points[2].y() * rect().height());
                painter.drawLine(p2, p3);
            }

            if (points[3] == NULL_POINTF) {
                painter.setPen(QPen(_releaseColor, 2));
                painter.drawLine(p2, QPoint(rect().width(), p2.y()));
            } else {
                painter.setPen(QPen(_releaseColor, 2));
                QPoint p4(points[3].x() * rect().height(), points[3].y() * rect().height());
                painter.drawLine(p2, p4);
            }
        }
    }

    painter.setPen(_borderColor);
    painter.drawRect(event->rect());
}

QList<QPointF> OPNEnvelopeDisplayWidget::getPoints() const
{
    QPointF p1(NULL_POINTF);
    QPointF p2(NULL_POINTF);
    QPointF p3(NULL_POINTF);
    QPointF p4(NULL_POINTF);


    if (_settings->arf() > 0) {
        p1.setY(_settings->t1lf());
        if (_settings->arf() == 1) {
            p1.setX(0);
        } else {
            const qreal theta = M_PI_2 * _settings->arf();
            p1.setX((1 - p1.y()) / qTan(theta));
        }

        if (_settings->d1rf() > 0) {
            p2.setY(p1.y() + _settings->t2lf() * (1 - p1.y()));
            if (_settings->d1rf() == 1) {
                p2.setX(p1.x());
            } else {
                const qreal theta = M_PI_2 * _settings->d1rf();
                p2.setX(p1.x() + (p2.y() - p1.y()) / qTan(theta));
            }

            if (_settings->d2rf() > 0) {
                p3.setY(1);
                if (_settings->d2rf() == 1) {
                    p3.setX(p2.x());
                } else {
                    const qreal theta = M_PI_2 * _settings->d2rf();
                    p3.setX(p2.x() + (1 - p2.y()) / qTan(theta));
                }
            }

            if (_settings->rrf() > 0) {
                p4.setY(1);
                if (_settings->rrf() == 1) {
                    p4.setX(p2.x());
                } else {
                    const qreal theta = M_PI_2 * _settings->rrf();
                    p4.setX(p2.x() + (1 - p2.y()) / qTan(theta));
                }

            }
        }
    }

    return QList<QPointF>({p1, p2, p3, p4});
}

const QColor& OPNEnvelopeDisplayWidget::backgroundColor() const
{
    return _backgroundColor;
}

const QColor& OPNEnvelopeDisplayWidget::borderColor() const
{
    return _borderColor;
}

const QColor& OPNEnvelopeDisplayWidget::envelopeColor() const
{
    return _envelopeColor;
}

const QColor& OPNEnvelopeDisplayWidget::levelColor() const
{
    return _levelColor;
}

const QColor& OPNEnvelopeDisplayWidget::releaseColor() const
{
    return _releaseColor;
}

void OPNEnvelopeDisplayWidget::setBackgroundColor(const QColor& color)
{
    _backgroundColor = color;
}

void OPNEnvelopeDisplayWidget::setBorderColor(const QColor& color)
{
    _borderColor = color;
}

void OPNEnvelopeDisplayWidget::setEnvelopeColor(const QColor& color)
{
    _envelopeColor = color;
}

void OPNEnvelopeDisplayWidget::setLevelColor(const QColor& color)
{
    _levelColor = color;
}

void OPNEnvelopeDisplayWidget::setReleaseColor(const QColor& color)
{
    _releaseColor = color;
}
