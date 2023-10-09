#include "prdisplaywidget.h"

constexpr QColor PRDisplayWidget::BorderColor;
constexpr QColor PRDisplayWidget::BackgroundColor;
constexpr QColor PRDisplayWidget::CursorColor;
constexpr QColor PRDisplayWidget::ItemColor;

PRDisplayWidget::PRDisplayWidget(QWidget* parent, Application* app, int index)
    : QWidget{parent}
    , _app(app)
    , _index(index)
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
}

void PRDisplayWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(BackgroundColor, Qt::SolidPattern));
    painter.setPen(QPen(BorderColor));
    painter.drawRect(rect());


    painter.setPen(QPen(ItemColor));
    float length = _app->project().getFrontPattern().getLength();
    int visibleLength = (qCeil(length /(float) _app->project().beatsPerBar())) * _app->project().beatsPerBar();
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
    float position = _app->position();

    if (_app->project().playMode() == Project::PlayMode::SONG) {
        QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_app->position());
        int frontPatternIdx = _app->project().frontPattern();

        if (activePatterns.contains(frontPatternIdx)) {
            position = _app->position() - activePatterns[frontPatternIdx];
        } else {
            position = 0;
        }
    }

    if (_app->isPlaying() && _app->project().getChannel(_index).enabled() && position <= visibleLength) {
        int appPositionPixel = position / beatsPerPixel;

        QPoint p1(appPositionPixel, 0);
        QPoint p2(appPositionPixel, height());

        painter.setPen(CursorColor);
        painter.drawLine(p1, p2);
    }
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
