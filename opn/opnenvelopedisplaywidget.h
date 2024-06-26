#ifndef OPNENVELOPEDISPLAYWIDGET_H
#define OPNENVELOPEDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QtMath>
#include <math.h>

#include "project/channel/opnenvelopesettings.h"

class OPNEnvelopeDisplayWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor envelopeColor READ envelopeColor WRITE setEnvelopeColor)
        Q_PROPERTY(QColor levelColor READ levelColor WRITE setLevelColor)
        Q_PROPERTY(QColor releaseColor READ releaseColor WRITE setReleaseColor)

    public:
        explicit OPNEnvelopeDisplayWidget(QWidget *parent = nullptr);

        void setSettings(const OPNEnvelopeSettings* settings);

    protected:
        virtual void paintEvent(QPaintEvent* event);

    signals:

    private:
        static QPointF NULL_POINTF;

        const OPNEnvelopeSettings* _settings;

        QList<QPointF> getPoints() const;

        QColor _backgroundColor;
        QColor _borderColor;
        QColor _envelopeColor;
        QColor _levelColor;
        QColor _releaseColor;

        const QColor& backgroundColor() const;
        const QColor& borderColor() const;
        const QColor& envelopeColor() const;
        const QColor& levelColor() const;
        const QColor& releaseColor() const;

        void setBackgroundColor(const QColor& color);
        void setBorderColor(const QColor& color);
        void setEnvelopeColor(const QColor& color);
        void setLevelColor(const QColor& color);
        void setReleaseColor(const QColor& color);

};

#endif // OPNENVELOPEDISPLAYWIDGET_H
