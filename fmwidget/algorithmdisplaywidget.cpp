#include "algorithmdisplaywidget.h"

QList<AlgorithmGraph> AlgorithmDisplayWidget::GRAPHS = {
    AlgorithmGraph(4, 1, "1;2;3;4", "1,2;2,3;3,4"),
    AlgorithmGraph(3, 2, "1,2;3;4", "1,3;2,3;3,4"),
    AlgorithmGraph(3, 2, "1,2;-1,3;4", "1,4;2,3;3,4"),
    AlgorithmGraph(3, 2, "1,-1;2,3;4", "1,2;2,4;3,4"),
    AlgorithmGraph(2, 2, "1,3;2,4", "1,2;3,4;"),
    AlgorithmGraph(2, 3, "1;2,3,4", "1,2;1,3;1,4"),
    AlgorithmGraph(2, 3, "1,-1,-1;2,3,4", "1,2"),
    AlgorithmGraph(1, 4, "1,2,3,4", "")
};

AlgorithmDisplayWidget::AlgorithmDisplayWidget(QWidget *parent)
    : QWidget(parent)
    , _algorithm(0)
    , _feedback(0)
    , _backgroundColor(Qt::white)
    , _borderColor(Qt::gray)
    , _operatorColor(QColor(0, 192, 255))
    , _operatorTextColor(Qt::blue)
    , _slotColor(Qt::blue)
    , _slotTextColor(Qt::white)
    , _edgeColor(Qt::blue)
{

}

void AlgorithmDisplayWidget::setAlgorithm(const int alg)
{
    _algorithm = alg;
    update();
}

void AlgorithmDisplayWidget::setFeedback(const char fb)
{
    _feedback = fb;
    update();
}

void AlgorithmDisplayWidget::drawArrow(QPainter& painter, const QPoint& start, const int width) const {
    const QLine line(start, QPoint(start.x() + width, start.y()));
    const QLine head1(line.p2(), line.p2() + QPoint(-4,-4));
    const QLine head2(line.p2(), line.p2() + QPoint(-4, 4));

    painter.drawLine(line);
    painter.drawLine(head1);
    painter.drawLine(head2);
}

void AlgorithmDisplayWidget::drawOutputEdges(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const {
    if (graph.slotOps().size() > 1) {
        const int midX = graph.connector(graph.slotOps().first(), AlgorithmGraph::Connector::Right).x() + (AlgorithmGraph::OP_PADDING / 2);
        QLine line(QPoint(midX, graph.connector(graph.slotOps().first(), AlgorithmGraph::Connector::Right).y()),
                   QPoint(midX, graph.connector(graph.slotOps().last(), AlgorithmGraph::Connector::Right).y()));

        for (const int slotOp : graph.slotOps()) {
            QPoint point = graph.connector(slotOp, AlgorithmGraph::Connector::Right);
            QLine line(point, point + QPoint(AlgorithmGraph::OP_PADDING / 2, 0));
            painter.drawLine(centered(event->rect(), graph.rect(), line));
        }

        painter.drawLine(centered(event->rect(), graph.rect(), line));
        drawArrow(painter, centered(event->rect(), graph.rect(), line.center()), AlgorithmGraph::OP_PADDING);
    } else {
        drawArrow(painter, centered(event->rect(), graph.rect(), graph.connector(graph.slotOps().first(), AlgorithmGraph::Connector::Right)), AlgorithmGraph::OP_PADDING);
    }
}

void AlgorithmDisplayWidget::drawNodes(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const {
    for (int i = 1; i <= 4; i++) {
        const QRect opRect(centered(event->rect(), graph.rect(), graph.topLeft(i)),
                            QSize(AlgorithmGraph::OP_SIZE, AlgorithmGraph::OP_SIZE));

        painter.setPen(graph.slotOps().contains(i) ? _slotTextColor : _operatorTextColor);
        painter.fillRect(opRect, QBrush(graph.slotOps().contains(i) ? _slotColor : _operatorColor, Qt::SolidPattern));
        painter.drawText(opRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(i));
        painter.setPen(_edgeColor);
        painter.drawRect(opRect);
    }
}

void AlgorithmDisplayWidget::drawEdges(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const {
    const QList<QLine> edges = graph.edges();
    for (const QLine& edge : edges) {
        QLine centeredEdge(centered(event->rect(), graph.rect(), edge));

        if (centeredEdge.p1().y() != centeredEdge.p2().y()) {
            int midX = centeredEdge.p2().x() - (AlgorithmGraph::OP_PADDING / 2);

            QLine line1(centeredEdge.p1(), QPoint(midX, centeredEdge.p1().y()));
            QLine line2(QPoint(midX, centeredEdge.p1().y()), QPoint(midX, centeredEdge.p2().y()));
            QLine line3(QPoint(midX, centeredEdge.p2().y()), centeredEdge.p2());

            painter.drawLine(line1);
            painter.drawLine(line2);
            painter.drawLine(line3);
        } else {
            painter.drawLine(centeredEdge);
        }
    }
}

void AlgorithmDisplayWidget::drawFeedback(QPaintEvent* event, QPainter& painter, const AlgorithmGraph& graph) const
{
    if (_feedback == 0)
        return;

    QPoint op1Top = centered(event->rect(), graph.rect(), graph.connector(1, AlgorithmGraph::Connector::Top));
    QPoint op1Left = centered(event->rect(), graph.rect(), graph.connector(1, AlgorithmGraph::Connector::Left));
    QLine right = QLine(op1Top, QPoint(op1Top.x(), op1Top.y() - 0.5 * AlgorithmGraph::OP_SIZE));
    QLine top = QLine(right.p2(), QPoint(right.p2().x() - AlgorithmGraph::OP_SIZE, right.p2().y()));
    QLine left = QLine(top.p2(), QPoint(top.p2().x(), op1Left.y()));

    painter.setPen(_edgeColor);
    painter.drawLine(right);
    painter.drawLine(top);
    painter.drawLine(left);
    drawArrow(painter, left.p2(), 0.5 * AlgorithmGraph::OP_SIZE);

    QPoint textRectBase = top.p2() + QPoint(4, 4);
    QRect textRext = QRect(textRectBase, QSize(16, 16));
    painter.drawText(textRext, Qt::AlignLeft | Qt::AlignTop, QString::number((int)_feedback));
}

void AlgorithmDisplayWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(event->rect(), QBrush(_backgroundColor, Qt::SolidPattern));
    painter.setPen(_borderColor);
    painter.drawRect(event->rect());

    const AlgorithmGraph& graph = GRAPHS[_algorithm];
    drawNodes(event, painter, graph);
    drawEdges(event, painter, graph);
    drawFeedback(event, painter, graph);
    drawOutputEdges(event, painter, graph);
}

const QColor& AlgorithmDisplayWidget::backgroundColor() const
{
    return _backgroundColor;
}

const QColor& AlgorithmDisplayWidget::borderColor() const
{
    return _borderColor;
}

const QColor& AlgorithmDisplayWidget::operatorColor() const
{
    return _operatorColor;
}

const QColor& AlgorithmDisplayWidget::operatorTextColor() const
{
    return _operatorTextColor;
}

const QColor& AlgorithmDisplayWidget::slotColor() const
{
    return _slotColor;
}

const QColor& AlgorithmDisplayWidget::slotTextColor() const
{
    return _slotTextColor;
}

const QColor& AlgorithmDisplayWidget::edgeColor() const
{
    return _edgeColor;
}

void AlgorithmDisplayWidget::setBackgroundColor(const QColor& color)
{
    _backgroundColor = color;
}

void AlgorithmDisplayWidget::setBorderColor(const QColor& color)
{
    _borderColor = color;
}

void AlgorithmDisplayWidget::setOperatorColor(const QColor& color)
{
    _operatorColor = color;
}

void AlgorithmDisplayWidget::setOperatorTextColor(const QColor& color)
{
    _operatorTextColor = color;
}

void AlgorithmDisplayWidget::setSlotColor(const QColor& color)
{
    _slotColor = color;
}

void AlgorithmDisplayWidget::setSlotTextColor(const QColor& color)
{
    _slotTextColor = color;
}

void AlgorithmDisplayWidget::setEdgeColor(const QColor& color)
{
    _edgeColor = color;
}

QPoint AlgorithmDisplayWidget::centered(const QRect& oRect, const QRect& iRect, const QPoint& point) const {
    return QPoint(
                point.x() + ((oRect.width() - iRect.width()) / 2),
                point.y() + ((oRect.height() - iRect.height()) / 2));
}

QLine AlgorithmDisplayWidget::centered(const QRect& oRect, const QRect& iRect, const QLine& line) const
{
    return QLine(centered(oRect, iRect, line.p1()), centered(oRect, iRect, line.p2()));
}
