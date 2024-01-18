#ifndef OPLENVELOPEDISPLAYWIDGET_H
#define OPLENVELOPEDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

#include "oplenvelopesettings.h"

class OPLEnvelopeDisplayWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor envelopeColor READ envelopeColor WRITE setEnvelopeColor)
        Q_PROPERTY(QColor levelColor READ levelColor WRITE setLevelColor)

    public:
        explicit OPLEnvelopeDisplayWidget(QWidget *parent = nullptr);

        void setSettings(const OPLEnvelopeSettings* settings);

    signals:
    private:
        static QPointF NULL_POINTF;

        const OPLEnvelopeSettings* _settings;

        QList<QPointF> getPoints() const;

        QColor _backgroundColor;
        QColor _borderColor;
        QColor _envelopeColor;
        QColor _levelColor;

        const QColor& backgroundColor() const;
        const QColor& borderColor() const;
        const QColor& envelopeColor() const;
        const QColor& levelColor() const;

        void setBackgroundColor(const QColor& color);
        void setBorderColor(const QColor& color);
        void setEnvelopeColor(const QColor& color);
        void setLevelColor(const QColor& color);


        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // OPLENVELOPEDISPLAYWIDGET_H
