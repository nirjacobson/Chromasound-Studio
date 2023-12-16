#ifndef ALGORITHMDISPLAYWIDGET_H
#define ALGORITHMDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QBrush>
#include <QLine>
#include <QList>

#include "algorithmgraph.h"

class AlgorithmDisplayWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor operatorColor READ operatorColor WRITE setOperatorColor)
        Q_PROPERTY(QColor operatorTextColor READ operatorTextColor WRITE setOperatorTextColor)
        Q_PROPERTY(QColor slotColor READ slotColor WRITE setSlotColor)
        Q_PROPERTY(QColor slotTextColor READ slotTextColor WRITE setSlotTextColor)
        Q_PROPERTY(QColor edgeColor READ edgeColor WRITE setEdgeColor)

    public:
        explicit AlgorithmDisplayWidget(QWidget *parent = nullptr);

        void setAlgorithm(const int alg);
        void setFeedback(const char fb);

    protected:
        void paintEvent(QPaintEvent* event);

    private:
        static QList<AlgorithmGraph> GRAPHS;

        int _algorithm;
        char _feedback;

        QColor _backgroundColor;
        QColor _borderColor;
        QColor _operatorColor;
        QColor _operatorTextColor;
        QColor _slotColor;
        QColor _slotTextColor;
        QColor _edgeColor;

        const QColor& backgroundColor() const;
        const QColor& borderColor() const;
        const QColor& operatorColor() const;
        const QColor& operatorTextColor() const;
        const QColor& slotColor() const;
        const QColor& slotTextColor() const;
        const QColor& edgeColor() const;

        void setBackgroundColor(const QColor& color);
        void setBorderColor(const QColor& color);
        void setOperatorColor(const QColor& color);
        void setOperatorTextColor(const QColor& color);
        void setSlotColor(const QColor& color);
        void setSlotTextColor(const QColor& color);
        void setEdgeColor(const QColor& color);

        QPoint centered(const QRect& oRect, const QRect& iRect, const QPoint& point) const;
        QLine centered(const QRect& oRect, const QRect& iRect, const QLine& line) const;

        void drawNodes(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
        void drawEdges(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
        void drawFeedback(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
        void drawOutputEdges(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
        void drawArrow(QPainter& painter, const QPoint& start, const int width) const;
};

#endif // ALGORITHMDISPLAYWIDGET_H
