#include "algorithmgraph.h"

AlgorithmGraph::AlgorithmGraph(const int cols, const int rows, const QString nodes, const QString edges)
    : _cols(cols)
    , _rows(rows)
{
    {
        std::stringstream ss(nodes.toStdString());
        std::string columnStr;

        QList<QList<int>> columns;
        while (std::getline(ss, columnStr, ';')) {
            std::stringstream ss1(columnStr);
            std::string itemStr;
            int item;

            QList<int> columnItems;
            while (std::getline(ss1, itemStr, ',')) {
                std::stringstream ss2(itemStr);
                ss2 >> item;
                columnItems.append(item);
            }

            columns.append(columnItems);
        }
        _nodes = columns;
    }

    {
        std::stringstream ss(edges.toStdString());
        std::string edgeStr;

        QList<QPair<int,int>> edges;
        while (std::getline(ss, edgeStr, ';')) {
            std::stringstream ss1(edgeStr);
            std::string itemStr;
            int item;

            QPair<int,int> edge;
            {
                std::getline(ss1, itemStr, ',');
                std::stringstream ss2(itemStr);
                ss2 >> item;
                edge.first = item;
            }
            {
                std::getline(ss1, itemStr, ',');
                std::stringstream ss2(itemStr);
                ss2 >> item;
                edge.second = item;
            }
            edges.append(edge);
        }
        _edges = edges;
    }

}

QRect AlgorithmGraph::rect() const
{
    return QRect(0, 0, width(), height());
}

int AlgorithmGraph::width() const
{
    return (OP_SIZE * _cols) + (OP_PADDING * (_cols - 1));
}

int AlgorithmGraph::height() const
{
    return (OP_SIZE * _rows) + (OP_PADDING * (_rows - 1));
}

QPoint AlgorithmGraph::topLeft(const int op) const
{
    for (int c = 0; c < _nodes.length(); c++) {
        for (int i = 0; i < _nodes[c].length(); i++) {
            if (_nodes[c][i] == op) {

                int left = c * (OP_SIZE + OP_PADDING);
                int top = i * (OP_SIZE + OP_PADDING);

                int translateY = (height() - ((OP_SIZE * _nodes[c].length()) + (OP_PADDING * (_nodes[c].length() - 1)))) / 2;

                return QPoint(left, top + translateY);
            }
        }
    }

    return QPoint(-1, -1);
}

QList<QLine> AlgorithmGraph::edges() const
{
    QList<QLine> edges;

    for (const QPair<int,int>& edge : _edges) {
        QLine line(connector(edge.first, Connector::Right),
                   connector(edge.second, Connector::Left));
        edges.append(line);
    }

    return edges;
}

const QList<int>& AlgorithmGraph::slotOps() const
{
    return _nodes.last();
}

QPoint AlgorithmGraph::connector(const int op, const AlgorithmGraph::Connector c) const
{
    QPoint topLeft = this->topLeft(op);

    switch (c) {
    case AlgorithmGraph::Left:
        return QPoint(topLeft.x(), topLeft.y() + (OP_SIZE / 2));
    case AlgorithmGraph::Right:
        return QPoint(topLeft.x() + OP_SIZE - 1, topLeft.y() + (OP_SIZE / 2));
    case AlgorithmGraph::Top:
        return QPoint(topLeft.x() + OP_SIZE / 2, topLeft.y());
    }

    return QPoint(-1, -1);
}

