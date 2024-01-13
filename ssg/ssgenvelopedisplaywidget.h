#ifndef SSGENVELOPEDISPLAYWIDGET_H
#define SSGENVELOPEDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

#include "ssg/ssgenvelopesettings.h"

class SSGEnvelopeDisplayWidget : public QWidget
{
        Q_OBJECT
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
};

#endif // SSGENVELOPEDISPLAYWIDGET_H
