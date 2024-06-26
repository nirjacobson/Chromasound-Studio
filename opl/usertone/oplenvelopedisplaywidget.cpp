#include "oplenvelopedisplaywidget.h"

QPointF OPLEnvelopeDisplayWidget::NULL_POINTF = QPointF(-1, -1);

OPLEnvelopeDisplayWidget::OPLEnvelopeDisplayWidget(QWidget *parent)
    : QWidget{parent}
    , _settings(nullptr)
    , _backgroundColor(Qt::white)
    , _borderColor(Qt::gray)
    , _envelopeColor(Qt::blue)
    , _levelColor(Qt::gray)
{

}

void OPLEnvelopeDisplayWidget::setSettings(const OPLEnvelopeSettings* settings)
{
    _settings = settings;
    update();
}

QList<QPointF> OPLEnvelopeDisplayWidget::getPoints() const
{
    QPointF p1(NULL_POINTF);
    QPointF p2(NULL_POINTF);
    QPointF p3(NULL_POINTF);
    QPointF p4(NULL_POINTF);

    if (_settings->arf() > 0) {
        p1.setY(_settings->tlf());
        if (_settings->arf() == 1) {
            p1.setX(0);
        } else {
            const qreal theta = M_PI_2 * _settings->arf();
            p1.setX((1 - p1.y()) / qTan(theta));
        }

        if (_settings->drf() > 0) {
            p2.setY(p1.y() + _settings->slf() * (1 - p1.y()));
            if (_settings->drf() == 1) {
                p2.setX(p1.x());
            } else {
                const qreal theta = M_PI_2 * _settings->drf();
                p2.setX(p1.x() + (p2.y() - p1.y()) / qTan(theta));
            }
        }

        if (_settings->type() == OPLEnvelopeSettings::EGType::Percussive) {
            if (_settings->rrf() > 0) {
                p3.setY(1);
                if (_settings->rrf() == 1) {
                    p3.setX(p2.x());
                } else {
                    const qreal theta = M_PI_2 * _settings->rrf();
                    p3.setX(p2.x() + (1 - p2.y()) / qTan(theta));
                }
            }
        } else {
            p3.setY(p2.y());
            p3.setX(p2.x() + 64.0f / height());

            if (_settings->rrf() > 0) {
                p4.setY(1);
                if (_settings->rrf() == 1) {
                    p4.setX(p3.x());
                } else {
                    const qreal theta = M_PI_2 * _settings->rrf();
                    p4.setX(p3.x() + (1 - p3.y()) / qTan(theta));
                }
            }
        }
    }

    return QList<QPointF>({p1, p2, p3, p4});
}

const QColor& OPLEnvelopeDisplayWidget::backgroundColor() const
{
    return _backgroundColor;
}

const QColor& OPLEnvelopeDisplayWidget::borderColor() const
{
    return _borderColor;
}

const QColor& OPLEnvelopeDisplayWidget::envelopeColor() const
{
    return _envelopeColor;
}

const QColor& OPLEnvelopeDisplayWidget::levelColor() const
{
    return _levelColor;
}

void OPLEnvelopeDisplayWidget::setBackgroundColor(const QColor& color)
{
    _backgroundColor = color;
}

void OPLEnvelopeDisplayWidget::setBorderColor(const QColor& color)
{
    _borderColor = color;
}

void OPLEnvelopeDisplayWidget::setEnvelopeColor(const QColor& color)
{
    _envelopeColor = color;
}

void OPLEnvelopeDisplayWidget::setLevelColor(const QColor& color)
{
    _levelColor = color;
}

void OPLEnvelopeDisplayWidget::paintEvent(QPaintEvent* event)
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

                if (points[3] == NULL_POINTF) {
                    painter.setPen(QPen(_envelopeColor, 2));
                    painter.drawLine(p3, QPoint(rect().width(), p3.y()));
                } else {
                    painter.setPen(QPen(_envelopeColor, 2));
                    QPoint p4(points[3].x() * rect().height(), points[3].y() * rect().height());
                    painter.drawLine(p3, p4);
                }
            }
        }
    }

    painter.setPen(_borderColor);
    painter.drawRect(event->rect());
}
