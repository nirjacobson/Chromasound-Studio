#ifndef ENVELOPEDISPLAYWIDGET_H
#define ENVELOPEDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QtMath>
#include <math.h>

#include "project/channel/envelopesettings.h"

class EnvelopeDisplayWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit EnvelopeDisplayWidget(QWidget *parent = nullptr);

        void setSettings(const EnvelopeSettings* settings);

    protected:
        virtual void paintEvent(QPaintEvent* event);

    signals:

    private:
        static QPointF NULL_POINTF;

        const EnvelopeSettings* _settings;

        QList<QPointF> getPoints() const;
};

#endif // ENVELOPEDISPLAYWIDGET_H
