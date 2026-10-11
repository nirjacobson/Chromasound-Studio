#ifndef COMPOSERDISPLAYWIDGET_H
#define COMPOSERDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>

class ComposerDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ComposerDisplayWidget(QWidget *parent = nullptr);
    ~ComposerDisplayWidget();
protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // COMPOSERDISPLAYWIDGET_H
