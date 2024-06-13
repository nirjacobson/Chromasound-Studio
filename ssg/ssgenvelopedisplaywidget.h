#ifndef SSGENVELOPEDISPLAYWIDGET_H
#define SSGENVELOPEDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

#include "ssg/ssgenvelopesettings.h"

class SSGEnvelopeDisplayWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor envelopeColor READ envelopeColor WRITE setEnvelopeColor)

    public:
        explicit SSGEnvelopeDisplayWidget(QWidget *parent = nullptr);

        void setSettings(const SSGEnvelopeSettings* settings);

    signals:

    protected:
        void paintEvent(QPaintEvent* event);

    private:
        static QPointF NULL_POINTF;
        static int PERIOD_WIDTH;

        const SSGEnvelopeSettings* _settings;

        QColor _backgroundColor;
        QColor _borderColor;
        QColor _envelopeColor;

        QList<QPointF> getPoints() const;

        const QColor& backgroundColor() const;
        void setBackgroundColor(const QColor& color);
        const QColor& borderColor() const;
        void setBorderColor(const QColor& color);
        const QColor& envelopeColor() const;
        void setEnvelopeColor(const QColor& color);
};

#endif // SSGENVELOPEDISPLAYWIDGET_H
