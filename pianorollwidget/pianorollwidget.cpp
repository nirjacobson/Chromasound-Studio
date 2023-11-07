#include "pianorollwidget.h"
#include "ui_pianorollwidget.h"

PianoRollWidget::PianoRollWidget(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , _app(app)
    , ui(new Ui::PianoRollWidget)
    , _keysWidget(new PianoRollKeysWidget(this))
    , _velocitiesWidget(new PianoRollVelocitiesWidget(this))
    , _itemLastClicked(nullptr)
    , _noteMenu(tr("Context menu"), this)
    , _velocityAction("Velocity", this)
{
    _velocitiesWidget->setApplication(app);

    ui->setupUi(this);

    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
    ui->actionSelectAll->setShortcuts(QKeySequence::SelectAll);
    ui->actionDelete->setShortcuts(QKeySequence::Delete);

    ui->ganttWidget->setApplication(_app);
    ui->ganttWidget->setLeftWidget(_keysWidget);
    ui->ganttWidget->setBottomWidget(_velocitiesWidget);
    ui->ganttWidget->setParameters(Rows, RowHeight, CellWidth, 0.25);
    ui->ganttWidget->invertRows(true);
    ui->ganttWidget->setItemsResizable(true);
    ui->ganttWidget->setItemsMovableX(true);
    ui->ganttWidget->setItemsMovableY(true);
    ui->ganttWidget->scrollVertically(0.25);
    ui->ganttWidget->setCellMajors({ 4, static_cast<int>(_app->project().beatsPerBar() / 0.25)});
    ui->ganttWidget->setRowMajors({ 12 });

    _noteMenu.addAction(&_velocityAction);

    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionCopy, &QAction::triggered, this, &PianoRollWidget::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &PianoRollWidget::paste);
    connect(ui->actionSelectAll, &QAction::triggered, this, &PianoRollWidget::selectAll);
    connect(ui->actionDelete, &QAction::triggered, this, &PianoRollWidget::deleteTriggered);

    connect(&_velocityAction, &QAction::triggered, this, &PianoRollWidget::velocityTriggered);

    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PianoRollWidget::ganttClicked);
    connect(ui->ganttWidget, &GanttWidget::itemChanged, this, &PianoRollWidget::ganttItemChanged);
    connect(ui->ganttWidget, &GanttWidget::itemReleased, this, &PianoRollWidget::ganttItemReleased);
    connect(ui->ganttWidget, &GanttWidget::contextMenuRequested, this, &PianoRollWidget::contextMenuRequested);

    connect(_keysWidget, &PianoRollKeysWidget::keyOn, this, &PianoRollWidget::keyOn);
    connect(_keysWidget, &PianoRollKeysWidget::keyOff, this, &PianoRollWidget::keyOff);
}

PianoRollWidget::~PianoRollWidget()
{
    delete ui;

    delete _velocitiesWidget;
    delete _keysWidget;
}

void PianoRollWidget::setTrack(const int pattern, const int track)
{
    _channel = track;
    _track = &_app->project().getPattern(pattern).getTrack(track);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&_track->items()));
    ui->ganttWidget->setPositionFunction([=](){
        float appPosition = _app->position();

        if (_app->project().playMode() == Project::PlayMode::SONG) {
            QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(appPosition);

            if (activePatterns.contains(pattern)) {
                float delta = appPosition - activePatterns[pattern];
                QList<int> activeTracks = _app->project().patterns()[pattern]->activeTracksAtTime(delta);

                if (activeTracks.contains(track)) {
                    return delta;
                }
            }
        } else {
            return appPosition;
        }

        return -1.0f;
    });
    update();
}

int PianoRollWidget::channel() const
{
    return _channel;
}

void PianoRollWidget::pressKey(const int key)
{
    _keysWidget->pressKey(key);
}

void PianoRollWidget::releaseKey(const int key)
{
    _keysWidget->releaseKey(key);
}

void PianoRollWidget::ganttClicked(Qt::MouseButton button, int row, float time)
{
    if (button == Qt::LeftButton) {
        _app->undoStack().push(new AddNoteCommand(_app->window(), *_track, time, Note(row, (_itemLastClicked ? _itemLastClicked->duration() : 1))));
    } else {
        _app->undoStack().push(new RemoveNoteCommand(_app->window(), *_track, time, Note(row, (_itemLastClicked ? _itemLastClicked->duration() : 1))));
    }
}

void PianoRollWidget::ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration)
{
    _app->undoStack().push(new EditNoteCommand(_app->window(), dynamic_cast<Track::Item*>(item), toTime, Note(toRow, toDuration, item->velocity()), reinterpret_cast<const QList<Track::Item*>&>(ui->ganttWidget->selectedItems())));
}

void PianoRollWidget::ganttItemReleased(const GanttItem* item)
{
    _itemLastClicked = item;
}

void PianoRollWidget::contextMenuRequested(GanttItem* item, const QPoint& location)
{
    _contextItem = dynamic_cast<Track::Item*>(item);
    _noteMenu.exec(location);
}

void PianoRollWidget::velocityTriggered()
{
    int velocity = QInputDialog::getInt(this, "Edit velocity", "Velocity", _contextItem->velocity(), 0, 100);

    Note n = _contextItem->note();
    n.setVelocity(velocity);

    _app->undoStack().push(new EditNoteCommand(_app->window(), _contextItem, _contextItem->time(), n, QList<Track::Item*>()));
}

void PianoRollWidget::copy()
{
    bson_writer_t* writer;

    uint8_t* buf = nullptr;
    size_t buflen = 0;
    bson_t* doc;

    writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

    bson_writer_begin(writer, &doc);

    bson_t items;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(doc, "trackItems", &items);
    for (const GanttItem* const item : ui->ganttWidget->selectedItems()) {
        const Track::Item* trackItem = dynamic_cast<const Track::Item*>(item);

        bson_t b_item;
        bson_init(&b_item);
        BSON::fromTrackItem(&b_item, trackItem);

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

void PianoRollWidget::paste()
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

    QList<Track::Item*> pastedItems;

    if (bson_iter_find_descendant(&iter, "trackItems", &items) && BSON_ITER_HOLDS_ARRAY(&items) && bson_iter_recurse(&items, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &item);
            pastedItems.append(new Track::Item(BSON::toTrackItem(item)));
        }

        _app->undoStack().push(new AddTrackItemsCommand(_app->window(), *_track, ui->ganttWidget->mousePosition(), pastedItems));

        ui->ganttWidget->selectItems(reinterpret_cast<QList<GanttItem*>&>(pastedItems));
        ui->ganttWidget->update();
    }

    bson_reader_destroy(reader);
}

void PianoRollWidget::selectAll()
{
    ui->ganttWidget->selectAllItems();
}

void PianoRollWidget::deleteTriggered()
{
    _app->undoStack().push(new RemoveTrackItemsCommand(_app->window(), *_track, reinterpret_cast<const QList<Track::Item*>&>(ui->ganttWidget->selectedItems())));
}

void PianoRollWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PianoRollWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

