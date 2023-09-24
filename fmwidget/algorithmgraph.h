#ifndef ALGORITHMGRAPH_H
#define ALGORITHMGRAPH_H

#include <QList>
#include <QPair>
#include <QLine>
#include <QRect>

#include <iostream>
#include <sstream>

class AlgorithmGraph
{
public:
    AlgorithmGraph(const int cols, const int rows, const QString nodes, const QString edges);

    static constexpr int OP_SIZE = 33;
    static constexpr int OP_PADDING = 15;

    enum Connector {
        Left,
        Right,
        Top,
    };

    QRect rect() const;

    int width() const;
    int height() const;

    QPoint topLeft(const int op) const;
    QPoint connector(const int op, const Connector c) const ;
    QList<QLine> edges() const;
    const QList<int>& slotOps() const;

private:
    int _cols;
    int _rows;
    QList<QList<int>> _nodes;
    QList<QPair<int,int>> _edges;

};

#endif // ALGORITHMGRAPH_H
