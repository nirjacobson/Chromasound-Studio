#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlaylistWidget::PlaylistWidget(QWidget *parent, Application* app) :
    QMainWindow(parent),
    _app(app),
    ui(new Ui::PlaylistWidget),
    _patternsWidget(new PlaylistPatternsWidget(this, app)),
    _loopColor(128, 192, 224),
    _appPosition(0),
    _markerMenu(tr("Context menu"), this),
    _lfoChangeAction("LFO frequency", this),
    _noiseFreqChangeAction("Noise frequency", this),
    _envFreqChangeAction("Envelope frequency", this),
    _envShapeChangeAction("Envelope shape", this),
    _userToneAction("User tone"),
    _editingLFOChange(nullptr),
    _editingNoiseFrequencyChange(nullptr),
    _editingEnvelopeFrequencyChange(nullptr),
    _editingEnvelopeShapeChange(nullptr)
{
    ui->setupUi(this);

    ui->changeWidget->setApplication(_app);

    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
    ui->actionSelectAll->setShortcuts(QKeySequence::SelectAll);
    ui->actionDelete->setShortcuts(QKeySequence::Delete);

    ui->ganttWidget->setApplication(_app);
    ui->ganttWidget->setDefaultCellWidth(CellWidth);
    ui->ganttWidget->setLeftWidget(_patternsWidget);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&app->project().playlist()));
    ui->ganttWidget->addMarkers(reinterpret_cast<QList<GanttMarker*>*>(&app->project().playlist().lfoChanges()));
    ui->ganttWidget->addMarkers(reinterpret_cast<QList<GanttMarker*>*>(&app->project().playlist().noiseFrequencyChanges()));
    ui->ganttWidget->addMarkers(reinterpret_cast<QList<GanttMarker*>*>(&app->project().playlist().envelopeFrequencyChanges()));
    ui->ganttWidget->addMarkers(reinterpret_cast<QList<GanttMarker*>*>(&app->project().playlist().envelopeShapeChanges()));
    ui->ganttWidget->addMarkers(reinterpret_cast<QList<GanttMarker*>*>(&app->project().playlist().userToneChanges()));
    ui->ganttWidget->setParameters(Rows, RowHeight, CellWidth, 1);
    ui->ganttWidget->setItemsMovableX(true);
    ui->ganttWidget->setPositionFunction([&]() {
        return _app->project().playMode() == Project::PlayMode::SONG ? _appPosition : 0.0f;
    });
    ui->ganttWidget->setHeaderPaintFunction([&](QPainter& painter, QRect rect, float leftPosition, float rightPosition, float beatsPerPixel) {
        if (_app->project().playlist().doesLoop()) {
            float loopOffset = _app->project().playlist().loopOffset();

            if (leftPosition <= loopOffset && loopOffset <= rightPosition) {
                int loopOffsetPixel = (loopOffset - leftPosition) / beatsPerPixel;

                QRect loopRect = QRect(QPoint(loopOffsetPixel, 0), QSize(24, 24));
                painter.setPen(QColor(_loopColor).darker());
                painter.setBrush(QColor(_loopColor));

                QPoint textPoint = loopRect.bottomLeft() + QPoint(4, -4);
                painter.fillRect(loopRect, painter.brush());
                painter.drawRect(loopRect);
                painter.drawText(textPoint, "L");
            }
        }
    });

    _markerMenu.addAction(&_lfoChangeAction);
    _markerMenu.addAction(&_noiseFreqChangeAction);
    _markerMenu.addAction(&_envFreqChangeAction);
    _markerMenu.addAction(&_envShapeChangeAction);
    _markerMenu.addAction(&_userToneAction);

    connect(ui->ganttWidget, &GanttWidget::markerClicked, this, &PlaylistWidget::ganttMarkerClicked);
    connect(ui->ganttWidget, &GanttWidget::headerClicked, this, &PlaylistWidget::ganttHeaderClicked);
    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PlaylistWidget::ganttEditorClicked);
    connect(ui->ganttWidget, &GanttWidget::itemChanged, this, &PlaylistWidget::ganttItemChanged);

    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionCopy, &QAction::triggered, this, &PlaylistWidget::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &PlaylistWidget::paste);
    connect(ui->actionSelectAll, &QAction::triggered, this, &PlaylistWidget::selectAll);
    connect(ui->actionDelete, &QAction::triggered, this, &PlaylistWidget::deleteTriggered);

    connect(ui->changeWidget, &PlaylistChangeWidget::doneButtonClicked, this, &PlaylistWidget::doneButtonClicked);
    connect(ui->changeWidget, &PlaylistChangeWidget::removeButtonClicked, this, &PlaylistWidget::removeButtonClicked);

    connect(_patternsWidget, &PlaylistPatternsWidget::patternClicked, this, &PlaylistWidget::patternClicked);

    connect(&_lfoChangeAction, &QAction::triggered, this, &PlaylistWidget::lfoChangeTriggered);
    connect(&_noiseFreqChangeAction, &QAction::triggered, this, &PlaylistWidget::noiseFreqChangeTriggered);
    connect(&_envFreqChangeAction, &QAction::triggered, this, &PlaylistWidget::envFreqChangeTriggered);
    connect(&_envShapeChangeAction, &QAction::triggered, this, &PlaylistWidget::envShapeChangeTriggered);
    connect(&_userToneAction, &QAction::triggered, this, &PlaylistWidget::userToneChangeTriggered);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
    delete _patternsWidget;
}

bool PlaylistWidget::hasLoop() const
{
    return ui->ganttWidget->hasLoop();
}

float PlaylistWidget::loopStart() const
{
    return ui->ganttWidget->loopStart();
}

float PlaylistWidget::loopEnd() const
{
    return ui->ganttWidget->loopEnd();
}

void PlaylistWidget::doUpdate(const float position)
{
    _patternsWidget->doUpdate(position);

    _appPosition = position;
    ui->ganttWidget->update();
    ui->changeWidget->doUpdate();
}

void PlaylistWidget::setCellMajors(const QList<int>& majors)
{
    ui->ganttWidget->setCellMajors(majors);
}

const QColor& PlaylistWidget::loopColor() const
{
    return _loopColor;
}

void PlaylistWidget::setLoopColor(const QColor& color)
{
    _loopColor = color;
}

const QColor& PlaylistWidget::lfoChangeColor() const
{
    return Playlist::LFOChange::COLOR;
}

void PlaylistWidget::setLFOChangeColor(const QColor& color)
{
    Playlist::LFOChange::COLOR = color;
}

const QColor& PlaylistWidget::noiseFreqChangeColor() const
{
    return Playlist::NoiseFrequencyChange::COLOR;
}

void PlaylistWidget::setNoiseFreqChangeColor(const QColor& color)
{
    Playlist::NoiseFrequencyChange::COLOR = color;
}

const QColor& PlaylistWidget::envFreqChangeColor() const
{
    return Playlist::EnvelopeFrequencyChange::COLOR;
}

void PlaylistWidget::setEnvFreqChangeColor(const QColor& color)
{
    Playlist::EnvelopeFrequencyChange::COLOR = color;
}

const QColor& PlaylistWidget::envShapeChangeColor() const
{
    return Playlist::EnvelopeShapeChange::COLOR;
}

void PlaylistWidget::setEnvShapeChangeColor(const QColor& color)
{
    Playlist::EnvelopeShapeChange::COLOR = color;
}

const QColor& PlaylistWidget::userToneChangeColor() const
{
    return Playlist::UserToneChange::COLOR;
}

void PlaylistWidget::setUserToneChangeColor(const QColor& color)
{
    Playlist::UserToneChange::COLOR = color;
}

void PlaylistWidget::ganttMarkerClicked(GanttMarker* marker)
{
    Playlist::LFOChange* lfoChange;
    Playlist::NoiseFrequencyChange* nfChange;
    Playlist::EnvelopeFrequencyChange* efChange;
    Playlist::EnvelopeShapeChange* esChange;
    Playlist::UserToneChange* utChange;

    if ((lfoChange = dynamic_cast<Playlist::LFOChange*>(marker))) {
        ui->changeWidget->setLFOChange(lfoChange);
        _editingLFOChange = lfoChange;
    } else if ((nfChange = dynamic_cast<Playlist::NoiseFrequencyChange*>(marker))) {
        ui->changeWidget->setNoiseFrequencyChange(nfChange);
        _editingNoiseFrequencyChange = nfChange;
    } else if ((efChange = dynamic_cast<Playlist::EnvelopeFrequencyChange*>(marker))) {
        ui->changeWidget->setEnvelopeFrequencyChange(efChange);
        _editingEnvelopeFrequencyChange = efChange;
    } else if ((esChange = dynamic_cast<Playlist::EnvelopeShapeChange*>(marker))) {
        ui->changeWidget->setEnvelopeShapeChange(esChange);
        _editingEnvelopeShapeChange = esChange;
    } else if ((utChange = dynamic_cast<Playlist::UserToneChange*>(marker))) {
        ui->changeWidget->setUserToneChange(utChange);
        _editingUserToneChange = utChange;
    }

    ui->stackedWidget->setCurrentIndex(1);

}

void PlaylistWidget::ganttHeaderClicked(Qt::MouseButton button, float time, QPoint location)
{
    if (button == Qt::RightButton) {
        if (time == _app->project().playlist().loopOffset()) {
            _app->project().playlist().setLoopOffset(-1);
        } else {
            _app->project().playlist().setLoopOffset(time);
        }
        update();
    } else {
        if (_app->project().playMode() == Project::PlayMode::SONG) {
            _app->setPosition(time);
        }

        if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
            _markerMenuTime = time;
            _markerMenu.exec(location);
        }

    }
}

void PlaylistWidget::ganttEditorClicked(Qt::MouseButton button, int row, float time)
{
    if (button == Qt::LeftButton) {
        if (_app->project().getPattern(row).getLength() > 0) {
            _app->undoStack().push(new AddPlaylistItemCommand(_app->window(), _app->project().playlist(), time, row));
        }
    } else {
        _app->undoStack().push(new RemovePlaylistItemCommand(_app->window(), _app->project().playlist(), time, row));
    }
}

void PlaylistWidget::ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration)
{
    _app->undoStack().push(new EditPlaylistCommand(_app->window(), dynamic_cast<Playlist::Item*>(item), toTime, reinterpret_cast<const QList<Playlist::Item*>&>(ui->ganttWidget->selectedItems())));
}

void PlaylistWidget::copy()
{
    bson_writer_t* writer;

    uint8_t* buf = nullptr;
    size_t buflen = 0;
    bson_t* doc;

    writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

    bson_writer_begin(writer, &doc);

    bson_t items;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(doc, "playlistItems", &items);
    for (const GanttItem* const item : ui->ganttWidget->selectedItems()) {
        const Playlist::Item* playlistItem = dynamic_cast<const Playlist::Item*>(item);

        bson_t b_item;
        bson_init(&b_item);
        BSON::fromPlaylistItem(&b_item, playlistItem);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&items, key, &b_item);
    }
    bson_append_array_end(doc, &items);

    bson_writer_end(writer);
    bson_writer_destroy(writer);

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/bson", QByteArray((const char*)buf, buflen));

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void PlaylistWidget::paste()
{
    QClipboard* clipboard = QGuiApplication::clipboard();

    const QMimeData* mimeData = clipboard->mimeData();
    if (!mimeData->hasFormat("application/bson")) {
        return;
    }

    QByteArray data = mimeData->data("application/bson");

    const uint8_t* cdata = reinterpret_cast<const uint8_t*>(data.constData());

    bson_reader_t* reader;
    const bson_t* b;

    reader = bson_reader_new_from_data(cdata, data.size());
    b = bson_reader_read(reader, NULL);

    bson_iter_t iter;
    bson_iter_init(&iter, b);

    bson_iter_t items;
    bson_iter_t child;
    bson_iter_t item;

    QList<Playlist::Item*> pastedItems;

    if (bson_iter_find_descendant(&iter, "playlistItems", &items) && BSON_ITER_HOLDS_ARRAY(&items) && bson_iter_recurse(&items, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &item);
            Playlist::Item* playlistItem = new Playlist::Item(BSON::toPlaylistItem(item, &_app->project()));
            pastedItems.append(playlistItem);
        }

        _app->undoStack().push(new AddPlaylistItemsCommand(_app->window(), _app->project().playlist(), ui->ganttWidget->mousePosition(), pastedItems));

        ui->ganttWidget->selectItems(reinterpret_cast<QList<GanttItem*>&>(pastedItems));
        ui->ganttWidget->update();
    }

    bson_reader_destroy(reader);
}

void PlaylistWidget::selectAll()
{
    ui->ganttWidget->selectAllItems();
}

void PlaylistWidget::deleteTriggered()
{
    _app->undoStack().push(new RemovePlaylistItemsCommand(_app->window(), _app->project().playlist(), reinterpret_cast<const QList<Playlist::Item*>&>(ui->ganttWidget->selectedItems())));
}

void PlaylistWidget::doneButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void PlaylistWidget::removeButtonClicked()
{
    if (_editingLFOChange) {
        _app->undoStack().push(new RemovePlaylistLFOChangeCommand(_app->window(), _app->project().playlist(), _editingLFOChange));
        _editingLFOChange = nullptr;
    }

    if (_editingNoiseFrequencyChange) {
        _app->undoStack().push(new RemovePlaylistNoiseFrequencyChangeCommand(_app->window(), _app->project().playlist(), _editingNoiseFrequencyChange));
        _editingNoiseFrequencyChange = nullptr;
    }

    if (_editingEnvelopeFrequencyChange) {
        _app->undoStack().push(new RemovePlaylistEnvelopeFrequencyChangeCommand(_app->window(), _app->project().playlist(), _editingEnvelopeFrequencyChange));
        _editingEnvelopeFrequencyChange = nullptr;
    }

    if (_editingEnvelopeShapeChange) {
        _app->undoStack().push(new RemovePlaylistEnvelopeShapeChangeCommand(_app->window(), _app->project().playlist(), _editingEnvelopeShapeChange));
        _editingEnvelopeShapeChange = nullptr;
    }

    if (_editingUserToneChange) {
        _app->undoStack().push(new RemovePlaylistUserToneChangeCommand(_app->window(), _app->project().playlist(), _editingUserToneChange));
        _editingUserToneChange = nullptr;
    }

    ui->stackedWidget->setCurrentIndex(0);
}

void PlaylistWidget::lfoChangeTriggered()
{
    _app->undoStack().push(new AddPlaylistLFOChangeCommand(_app->window(), _app->project().playlist(), _markerMenuTime, 0));
}

void PlaylistWidget::noiseFreqChangeTriggered()
{
    _app->undoStack().push(new AddPlaylistNoiseFrequencyChangeCommand(_app->window(), _app->project().playlist(), _markerMenuTime, 0));
}

void PlaylistWidget::envFreqChangeTriggered()
{
    _app->undoStack().push(new AddPlaylistEnvelopeFrequencyChangeCommand(_app->window(), _app->project().playlist(), _markerMenuTime, 0));
}

void PlaylistWidget::envShapeChangeTriggered()
{
    _app->undoStack().push(new AddPlaylistEnvelopeShapeChangeCommand(_app->window(), _app->project().playlist(), _markerMenuTime));
}

void PlaylistWidget::userToneChangeTriggered()
{
    _app->undoStack().push(new AddPlaylistUserToneChangeCommand(_app->window(), _app->project().playlist(), _markerMenuTime));
}

void PlaylistWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PlaylistWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

