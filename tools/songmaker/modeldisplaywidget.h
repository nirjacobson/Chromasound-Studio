#ifndef MODELDISPLAYWIDGET_H
#define MODELDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>

class ModelDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ModelDisplayWidget(QWidget *parent = nullptr);

    void setLayerSizes(const std::vector<int>& layerSizes);
signals:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);

private:
    std::vector<int> _layerSizes;

    QSize figureSize();
};

#endif // MODELDISPLAYWIDGET_H
