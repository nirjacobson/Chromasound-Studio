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

    QPoint centered(const QRect& oRect, const QRect& iRect, const QPoint& point) const;
    QLine centered(const QRect& oRect, const QRect& iRect, const QLine& line) const;

    void drawNodes(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
    void drawEdges(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
    void drawFeedback(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
    void drawOutputEdges(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const;
    void drawArrow(QPainter& painter, const QPoint& start, const int width) const;
};

#endif // ALGORITHMDISPLAYWIDGET_H
