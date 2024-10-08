#include "prdisplaywidget.h"

PRDisplayWidget::PRDisplayWidget(QWidget* parent, Application* app, int index)
    : DamageWidget{parent}
    , _app(app)
    , _index(index)
    , _borderColor(QColor(128,128,128))
    , _backgroundColor(QColor(255,255,255))
    , _cursorColor(QColor(64, 192, 64))
    , _itemColor(QColor(128, 128, 255))
    , _appPosition(0)
{
    setMinimumWidth(192);
}

void PRDisplayWidget::setApplication(Application* app)
{
    _app = app;
}

void PRDisplayWidget::setIndex(const int i)
{
    _index = i;

    setNeedsFullPaint();
    update();
}

void PRDisplayWidget::doUpdate(const float position, const bool full)
{
    _appPosition = position;

    if (full) {
        setNeedsFullPaint();
    }

    update();
}

void PRDisplayWidget::mousePressEvent(QMouseEvent* event)
{
    emit pianoRollTriggered();
}

const QColor& PRDisplayWidget::borderColor() const
{
    return _borderColor;
}

const QColor& PRDisplayWidget::backgroundColor() const
{
    return _backgroundColor;
}

const QColor& PRDisplayWidget::cursorColor() const
{
    return _cursorColor;
}

const QColor& PRDisplayWidget::itemColor() const
{
    return _itemColor;
}

void PRDisplayWidget::setBorderColor(const QColor& color)
{
    _borderColor = color;
}

void PRDisplayWidget::setBackgroundColor(const QColor& color)
{
    _backgroundColor = color;
}

void PRDisplayWidget::setCursorColor(const QColor& color)
{
    _cursorColor = color;
}

void PRDisplayWidget::setItemColor(const QColor& color)
{
    _itemColor = color;
}

QPair<int, int> PRDisplayWidget::range() const
{
    QList<Track::Item*>& items = _app->project().getFrontPattern().getTrack(_index).items();

    int low = items.empty() ? 0 : items[0]->note().key();
    int high = items.empty() ? 0 : items[0]->note().key();
    for (const Track::Item* item : items) {
        if (item->note().key() < low) {
            low = item->note().key();
        }
        if (item->note().key() > high) {
            high = item->note().key();
        }
    }

    return QPair<int, int>(low, high);
}

void PRDisplayWidget::paintFull(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(_backgroundColor, Qt::SolidPattern));
    painter.setPen(QPen(_borderColor));
    painter.drawRect(rect());


    painter.setPen(QPen(_itemColor));
    int visibleLength =  _app->project().getPatternBarLength(_app->project().frontPattern());
    int visibleHeight = (float)height() * 0.8f;

    int r = range().second - range().first;

    QList<Track::Item*>& items = _app->project().getFrontPattern().getTrack(_index).items();
    for (const Track::Item* item : items) {
        int x = (item->time() / (float)visibleLength) * width();
        int w = (item->duration() / (float)visibleLength) * width();
        int y = r ? ((float)(item->note().key() - range().first) / (float)r) * visibleHeight : 0;
        int o = (float)(height() - visibleHeight) / 2.0f;
        y  = height() - (y + o);

        painter.drawLine(QPoint(x, y), QPoint(x+w, y));
    }

    float beatsPerPixel = (float)visibleLength / (float)width();
    float position = _appPosition;

    if (_app->project().playMode() == Project::PlayMode::SONG) {
        QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_appPosition);
        int frontPatternIdx = _app->project().frontPattern();

        if (activePatterns.contains(frontPatternIdx)) {
            position = _appPosition - activePatterns[frontPatternIdx];
        } else {
            position = 0;
        }
    }
}

void PRDisplayWidget::paintPartial(QPaintEvent* event)
{
    QPainter painter(this);

    int visibleLength =  _app->project().getPatternBarLength(_app->project().frontPattern());
    float beatsPerPixel = (float)visibleLength / (float)width();
    float position = -1;

    if (_app->project().playMode() == Project::PlayMode::PATTERN) {
        position = _appPosition;
    } else {
        QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_appPosition);
        int frontPatternIdx = _app->project().frontPattern();
        if (activePatterns.contains(frontPatternIdx)) {
            position = (_appPosition - activePatterns[frontPatternIdx]);
        }
    }

    if (_app->isPlaying() && _app->project().getChannel(_index).enabled() && position <= visibleLength) {
        int appPositionPixel = position / beatsPerPixel;

        QPoint p1(appPositionPixel, 0);
        QPoint p2(appPositionPixel, height());

        painter.setPen(_cursorColor);
        painter.drawLine(p1, p2);
    }
}
