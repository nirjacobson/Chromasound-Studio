#ifndef PCMUSAGELEGENDWIDGET_H
#define PCMUSAGELEGENDWIDGET_H

#include <QWidget>
#include <QPainter>

#include "pcmusagedisplayitem.h"

class PCMUsageLegendWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)

    public:
        explicit PCMUsageLegendWidget(QWidget *parent = nullptr);

        void setItems(const QList<PCMUsageDisplayItem>* items);

    signals:

    private:
        const QList<PCMUsageDisplayItem>* _items;
        QColor _foregroundColor;

        const QColor& foregroundColor() const;
        void setForegroundColor(const QColor& color);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // PCMUSAGELEGENDWIDGET_H
