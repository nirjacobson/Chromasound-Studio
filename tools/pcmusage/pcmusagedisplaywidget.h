#ifndef PCMUSAGEDISPLAYWIDGET_H
#define PCMUSAGEDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>

#include "pcmusagedialog.h"
#include "pcmusagedisplayitem.h"

class PCMUsageDisplayWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)

    public:
        explicit PCMUsageDisplayWidget(QWidget *parent = nullptr);

        void setItems(const QList<PCMUsageDisplayItem>* items);

    signals:

    private:
        QColor _outlineColor;

        const QList<PCMUsageDisplayItem>* _items;

        const QColor& outlineColor() const;
        void setOutlineColor(const QColor& color);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // PCMUSAGEDISPLAYWIDGET_H
