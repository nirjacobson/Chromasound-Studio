#include "modeldisplaywidget.h"

ModelDisplayWidget::ModelDisplayWidget(QWidget *parent)
    : QWidget{parent}
{
    _layerSizes = {2, 3, 4};
}

void ModelDisplayWidget::setLayerSizes(const std::vector<int> &layerSizes)
{
    _layerSizes = layerSizes;
    update();
}

void ModelDisplayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::blue, 2));
    painter.setBrush(QBrush(Qt::white));

    painter.fillRect(rect(), Qt::white);

    QList<QPoint> previousPoints;
    QList<QPoint> newPoints;

    int intnl_layers = _layerSizes.size();
    int total_layers = intnl_layers + 4;

    int node_padding_y = height() / 10;
    int max_nodes_in_layer = *std::max_element(_layerSizes.begin(), _layerSizes.end());
    int node_diameter = qMin(32, qMax(2, (height() - ((max_nodes_in_layer - 1) * node_padding_y)) / max_nodes_in_layer));
    int node_padding_x = width() / 20;

    QString text = "x1";
    int textWidth = painter.fontMetrics().horizontalAdvance(text);
    int textHeight = painter.fontMetrics().height();

    int x = ((width() - figureSize().width()) / 2) + 24;
    int y = (height() / 2);

    y -= 32;

    previousPoints.push_back(QPoint(x + ((node_diameter + node_padding_x) / 4), y));

    painter.save();
    painter.translate(x, y);
    QRect text_rect = QRect((-textWidth / 2), (-textHeight / 2), textWidth, textHeight);
    painter.drawText(text_rect, text);
    painter.restore();

    y += 32;

    previousPoints.push_back(QPoint(x + ((node_diameter + node_padding_x) / 4), y));

    text = "•••";
    textWidth = painter.fontMetrics().horizontalAdvance(text);
    textHeight = painter.fontMetrics().height();

    painter.save();
    painter.translate(x, y);
    painter.rotate(270);
    text_rect = QRect((-textWidth / 2), (-textHeight / 2), textWidth, textHeight);
    painter.drawText(text_rect, text);
    painter.restore();

    y += 32;

    previousPoints.push_back(QPoint(x + ((node_diameter + node_padding_x) / 4), y));

    text = "xn";
    textWidth = painter.fontMetrics().horizontalAdvance(text);
    textHeight = painter.fontMetrics().height();

    painter.save();
    painter.translate(x, y);
    text_rect = QRect((-textWidth / 2), (-textHeight / 2), textWidth, textHeight);
    painter.drawText(text_rect, text);
    painter.restore();

    painter.setPen(QPen(painter.pen().color(), 1));
    for (int i = 0; i < intnl_layers; i++) {
        for (int j = 0; j < _layerSizes[i]; j++) {
            int x = ((width() - figureSize().width()) / 2) + (48 + node_padding_x) + (((node_diameter + node_padding_x) * i));
            int y = ((height() - (node_diameter * _layerSizes[i]) - (node_padding_y * (_layerSizes[i] - 1))) / 2) + ((node_diameter + node_padding_y) * j);
            newPoints.push_back(QPoint(x + ((1 * node_diameter)/2), y + (node_diameter / 2)));
            for (int k = 0; k < previousPoints.size(); k++) {
                painter.drawLine(previousPoints[k], QPoint(x + (node_diameter / 2), y + (node_diameter / 2)));
            }
        }
        previousPoints = newPoints;
        newPoints.clear();
    }

    x = ((width() - figureSize().width()) / 2) + (48 + node_padding_x) + (((node_diameter + node_padding_x) * intnl_layers));
    y = node_padding_y;

    int x_prev = x;
    x = x_prev;
    painter.setPen(QPen(painter.pen().color(), 1));
    for (int i = 0; i < 2; i++) {
        x += i * ((width() / 24) + node_padding_x);

        for (int k = 0; k < previousPoints.size(); k++) {
            painter.drawLine(previousPoints[k], QPoint(x + ((width() / 24) / 2), height() / 2));
        }

        newPoints = {QPoint(x + ((width() / 24) / 2), height() / 2)};
        previousPoints = newPoints;
        newPoints.clear();
    }

    newPoints = {previousPoints[0] + QPoint((width() / 24) + node_padding_x, 0)};
    painter.drawLine(previousPoints[0], newPoints[0]);
    painter.drawLine(newPoints[0], newPoints[0] + QPoint(-8, -8));
    painter.drawLine(newPoints[0], newPoints[0] + QPoint(-8,  8));

    painter.setPen(QPen(painter.pen().color(), 2));
    for (int i = 0; i < intnl_layers; i++) {
        for (int j = 0; j < _layerSizes[i]; j++) {
            int x = ((width() - figureSize().width()) / 2) + (48 + node_padding_x) + (((node_diameter + node_padding_x) * i));
            int y = ((height() - (node_diameter * _layerSizes[i]) - (node_padding_y * (_layerSizes[i] - 1))) / 2) + ((node_diameter + node_padding_y) * j);

            QRect node_rect(QPoint(x, y), QSize(node_diameter, node_diameter));

            painter.drawEllipse(node_rect);
        }
    }

    x = x_prev;
    painter.setPen(QPen(painter.pen().color(), 2));
    for (int i = 0; i < 2; i++) {
        x += i * ((width() / 24) + node_padding_x);
        QRect layer_rect(QPoint(x, y), QSize(width() / 24, (height() - (2 * node_padding_y))));
        painter.drawRect(layer_rect);

        int text_x = x + ((width() / 24) / 2);
        int text_y = height() / 2;

        painter.save();
        painter.translate(text_x, text_y);
        painter.rotate(270);
        QString text = i == 0 ? "tanh" : "softmax";
        int textWidth = painter.fontMetrics().horizontalAdvance(text);
        int textHeight = painter.fontMetrics().height();
        QRect text_rect = QRect((-textWidth / 2), (-textHeight / 2), textWidth, textHeight);
        painter.drawText(text_rect, Qt::AlignCenter, text);
        painter.restore();
    }
}

QSize ModelDisplayWidget::figureSize()
{
    int intnl_layers = _layerSizes.size();
    int total_layers = intnl_layers + 4;

    int node_padding_y = height() / 10;
    int max_nodes_in_layer = *std::max_element(_layerSizes.begin(), _layerSizes.end());
    int node_diameter = qMin(32, qMax(2, (height() - ((max_nodes_in_layer - 1) * node_padding_y)) / max_nodes_in_layer));
    int node_padding_x = width() / 20;

    int figureWidth = 48 + (intnl_layers * node_diameter) + (3 * (width() / 24)) + ((total_layers - 1) * node_padding_x);
    int figureHeight = (max_nodes_in_layer * node_diameter) + ((max_nodes_in_layer - 1) * node_padding_y);

    return QSize(figureWidth, figureHeight);
}
