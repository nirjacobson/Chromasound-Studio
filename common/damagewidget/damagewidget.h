#ifndef DAMAGEWIDGET_H
#define DAMAGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class DamageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DamageWidget(QWidget *parent = nullptr);
    ~DamageWidget();

    void setNeedsFullPaint();

private:
    bool _needsFullPaint;
    bool _gettingPixmap;
    QPixmap* _lastFullRender;

    virtual void paintFull(QPaintEvent* event) = 0;
    virtual void paintPartial(QPaintEvent* event) = 0;

    QPixmap* getFullRender();
signals:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
};

#endif // DAMAGEWIDGET_H
