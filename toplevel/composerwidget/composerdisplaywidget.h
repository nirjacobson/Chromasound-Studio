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

    void show(QPixmap* pixmap);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QPixmap* _pixmap;

};

#endif // COMPOSERDISPLAYWIDGET_H
