#include "playlistpatternswidget.h"

PlaylistPatternsWidget::PlaylistPatternsWidget(QWidget *parent, Application* app)
    : GanttLeftWidget{parent}
    , _app(app)
    , _rows(0)
    , _top(0)
    , _rowHeight(16)
    , _ledColor(Qt::green)
    , _foregroundColor(Qt::gray)
{
    setMinimumWidth(128);
    setMaximumWidth(128);
}

float PlaylistPatternsWidget::getScrollPercentage()
{
    int scrollHeight = qMax(0, length() - height());

    return (float)_top / (float)scrollHeight;
}

void PlaylistPatternsWidget::setScrollPercentage(const float percent)
{
    int scrollHeight = qMax(0, length() - height());
    _top = percent * scrollHeight;

    update();
}

void PlaylistPatternsWidget::scrollBy(const int pixels)
{
    _top += pixels;
    update();
}

void PlaylistPatternsWidget::setRows(int rows)
{
    _rows = rows;
}

void PlaylistPatternsWidget::setRowHeight(int height)
{
    _rowHeight = height;
}

int PlaylistPatternsWidget::length() const
{
    return _rowHeight * _rows;
}

void PlaylistPatternsWidget::paintEvent(QPaintEvent*)
{
    int firstPattern = _top / _rowHeight;
    int firstPatternStart = firstPattern * _rowHeight - _top;

    int numPatternsAcrossHeight = qCeil((float)height()/_rowHeight) + 1;

    QPainter painter(this);

    QPoint topLeft(0, firstPatternStart);

    float appPosition = _app->position();

    QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(appPosition);


    for (int i = 0; i < numPatternsAcrossHeight && (firstPattern + i) < _rows; i++) {
        int pattern = firstPattern + i;

        bool on = false;
        if (_app->project().playMode() == Project::PlayMode::SONG && activePatterns.contains(pattern)) {
            Pattern& pat = _app->project().getPattern(pattern);
            QList<int> activeTracks = pat.activeTracksAtTime(appPosition - activePatterns[pattern]);
            for (int t : activeTracks) {
                Track& track = pat.getTrack(t);
                bool noneAreInDelta = std::find_if(track.items().begin(), track.items().end(),
                                            [&](const Track::Item* item){
                                              float delta = item->time() - (appPosition - activePatterns[pattern]);
                                              return qAbs(delta)<= 0.0625; }) == track.items().end();
                if (noneAreInDelta) {
                    on = true;
                    break;
                }
            }
        }

        QPoint thisTopLeft = topLeft + QPoint(0, i * _rowHeight);
        QRect rect(thisTopLeft, thisTopLeft + QPoint(width() - 2, _rowHeight));

        painter.setPen(_foregroundColor);
        painter.setBrush(QColor(QWidget::palette().color(QWidget::backgroundRole())));
        painter.fillRect(rect, painter.brush());
        painter.drawRect(rect);
        painter.drawText(rect.adjusted(4, 4, 0, 0), QString("Pattern %1").arg(firstPattern + i + 1));

        rect.setTopLeft(QPoint(width() - 2 - LED_WIDTH, thisTopLeft.y()));

        QColor ledColor = on ? _ledColor : _ledColor.darker();

        painter.setPen(ledColor.darker());
        painter.setBrush(ledColor);
        painter.fillRect(rect, painter.brush());
        painter.drawRect(rect);
    }
}

const QColor& PlaylistPatternsWidget::foregroundColor() const
{
    return _foregroundColor;
}

const QColor& PlaylistPatternsWidget::ledColor() const
{
    return _ledColor;
}

void PlaylistPatternsWidget::setForegroundColor(const QColor& color)
{
    _foregroundColor = color;
}

void PlaylistPatternsWidget::setLEDColor(const QColor& color)
{
    _ledColor = color;
}
