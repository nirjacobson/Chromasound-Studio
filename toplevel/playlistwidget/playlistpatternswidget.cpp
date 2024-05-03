#include "playlistpatternswidget.h"

PlaylistPatternsWidget::PlaylistPatternsWidget(QWidget *parent, Application* app)
    : GanttLeftWidget{parent}
    , _app(app)
    , _rows(0)
    , _top(0)
    , _rowHeight(16)
    , _ledColor(Qt::green)
    , _appPosition(0)
    , _contextMenu(tr("Context menu"), this)
    , _moveUpAction("Move up", this)
    , _moveDownAction("Move down", this)
    , _duplicateAction("Duplicate", this)
    , _insertAction("Insert new", this)
    , _deleteAction("Delete", this)
    , _contextPattern(0)
{
    setMinimumWidth(128);
    setMaximumWidth(128);

    connect(&_moveUpAction, &QAction::triggered, this, &PlaylistPatternsWidget::moveUpTriggered);
    connect(&_moveDownAction, &QAction::triggered, this, &PlaylistPatternsWidget::moveDownTriggered);
    connect(&_duplicateAction, &QAction::triggered, this, &PlaylistPatternsWidget::duplicateTriggered);
    connect(&_insertAction, &QAction::triggered, this, &PlaylistPatternsWidget::insertTriggered);
    connect(&_deleteAction, &QAction::triggered, this, &PlaylistPatternsWidget::deleteTriggered);

    _contextMenu.addAction(&_moveUpAction);
    _contextMenu.addAction(&_moveDownAction);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_duplicateAction);
    _contextMenu.addAction(&_deleteAction);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_insertAction);

    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &PlaylistPatternsWidget::contextMenuRequested);
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

void PlaylistPatternsWidget::doUpdate(const float position)
{
    _appPosition = position;
    update();
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
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint topLeft(0, firstPatternStart);

    QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_appPosition);


    for (int i = 0; i < numPatternsAcrossHeight && (firstPattern + i) < _rows; i++) {
        int pattern = firstPattern + i;

        bool on = false;
        if (_app->project().playMode() == Project::PlayMode::SONG && activePatterns.contains(pattern)) {
            Pattern& pat = _app->project().getPattern(pattern);
            QList<int> activeTracks = pat.activeTracksAtTime(_appPosition - activePatterns[pattern]);
            for (int t : activeTracks) {
                if (!_app->project().getChannel(t).enabled()) {
                    continue;
                }

                Track& track = pat.getTrack(t);
                bool noneAreInDelta = std::find_if(track.items().begin(), track.items().end(),
                [&](const Track::Item* item) {
                    float delta = item->time() - (_appPosition - activePatterns[pattern]);
                    return qAbs(delta)<= 0.0625;
                }) == track.items().end();
                if (noneAreInDelta) {
                    on = true;
                    break;
                }
            }
        }

        QColor color = QWidget::palette().color(QWidget::backgroundRole());

        QPoint thisTopLeft = topLeft + QPoint(0, i * _rowHeight);

        QRect rect(thisTopLeft, thisTopLeft + QPoint(_rowHeight, _rowHeight));
        painter.setPen(color.lightness() <= 128 ? color.lighter() : color.darker());
        painter.setBrush(color);
        painter.fillRect(rect, painter.brush());
        painter.drawRect(rect);
        painter.drawText(rect.adjusted(4, 4, -6, -4), Qt::AlignRight, QString("%1").arg(pattern + 1));

        rect = QRect(rect.topRight(), thisTopLeft + QPoint(width() - 2, _rowHeight));
        painter.setPen(color.lightness() <= 128 ? color.lighter() : color.darker());
        painter.setBrush(color);
        painter.fillRect(rect, painter.brush());
        painter.drawRect(rect);

        QString name = _app->project().getPattern(pattern).name();
        painter.drawText(rect.adjusted(4, 4, 0, 0), name.isEmpty()
                                                        ? QString("Pattern %1").arg(pattern + 1)
                                                        : name);

        rect.setTopLeft(QPoint(width() - 2 - LED_WIDTH, thisTopLeft.y()));

        QColor ledColor = on ? _ledColor : _ledColor.darker();

        painter.setPen(ledColor.darker());
        painter.setBrush(ledColor);
        painter.fillRect(rect, painter.brush());
        painter.drawRect(rect);
    }
}

void PlaylistPatternsWidget::mousePressEvent(QMouseEvent* event)
{
    int firstPattern = _top / _rowHeight;
    int firstPatternStart = firstPattern * _rowHeight - _top;

    int pattern = firstPattern + ((event->pos().y() - firstPatternStart) / _rowHeight);

    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        bool ok;
        const QString name = QInputDialog::getText(this, tr("Change Pattern Name"), tr("Pattern name:"), QLineEdit::Normal, _app->project().getPattern(pattern).name(), &ok);
        if (ok && !name.isEmpty()) {
            _app->undoStack().push(new SetPatternNameCommand(_app->window(), _app->project().getPattern(pattern), name));
        }
    } else {
        emit patternClicked(pattern + 1);
    }
}

void PlaylistPatternsWidget::contextMenuRequested(const QPoint& p)
{
    int firstPattern = _top / _rowHeight;
    int firstPatternStart = firstPattern * _rowHeight - _top;

    _contextPattern = firstPattern + ((p.y() - firstPatternStart) / _rowHeight);

    _contextMenu.exec(mapToGlobal(p));
}

void PlaylistPatternsWidget::moveUpTriggered()
{
    _app->undoStack().push(new MovePatternUpCommand(_app->window(), _contextPattern));
}

void PlaylistPatternsWidget::moveDownTriggered()
{
    _app->undoStack().push(new MovePatternDownCommand(_app->window(), _contextPattern));
}

void PlaylistPatternsWidget::duplicateTriggered()
{
    _app->undoStack().push(new DuplicatePatternCommand(_app->window(), _contextPattern));
}

void PlaylistPatternsWidget::insertTriggered()
{
    _app->undoStack().push(new InsertPatternCommand(_app->window(), _contextPattern));
}

void PlaylistPatternsWidget::deleteTriggered()
{
    _app->undoStack().push(new DeletePatternCommand(_app->window(), _contextPattern));
}

const QColor& PlaylistPatternsWidget::ledColor() const
{
    return _ledColor;
}

void PlaylistPatternsWidget::setLEDColor(const QColor& color)
{
    _ledColor = color;
}
