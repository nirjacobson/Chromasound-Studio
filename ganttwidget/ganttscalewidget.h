#ifndef GANTTSCALEWIDGET_H
#define GANTTSCALEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

class GanttScaleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GanttScaleWidget(QWidget *parent = nullptr);

signals:
    void scaleChanged(float scale);

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    static constexpr float MinScale = 0.1;
    static constexpr float MaxScale = 4.0;
    static constexpr float ScalePerPixel = 0.1;

    float _scale;
};

#endif // GANTTSCALEWIDGET_H
