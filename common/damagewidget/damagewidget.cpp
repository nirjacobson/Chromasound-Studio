#include "damagewidget.h"

DamageWidget::DamageWidget(QWidget *parent)
    : QWidget{parent}
    , _needsFullPaint(true)
    , _gettingPixmap(false)
    , _lastFullRender(nullptr)
{}

DamageWidget::~DamageWidget()
{
    if (_lastFullRender) delete _lastFullRender;
}

void DamageWidget::setNeedsFullPaint()
{
    _needsFullPaint = true;
}

QPixmap* DamageWidget::getFullRender()
{
    _gettingPixmap = true;

    QPixmap* theRender = new QPixmap(size());

    render(theRender);

    _gettingPixmap = false;

    return theRender;
}

void DamageWidget::paintEvent(QPaintEvent* event)
{
    if (_needsFullPaint) {
        paintFull(event);

        if (!_gettingPixmap) {
            if (_lastFullRender) {
                delete _lastFullRender;
                _lastFullRender = nullptr;
            }

            _lastFullRender = getFullRender();
            _needsFullPaint = false;
            update();
        }
    } else {
        QPainter painter(this);
        painter.drawImage(QPoint(), _lastFullRender->toImage());

        paintPartial(event);
    }
}

void DamageWidget::resizeEvent(QResizeEvent* event)
{
    _needsFullPaint = true;
}
