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

    ui->fmWidget->setApplication(app);
    ui->noiseWidget->setApplication(app);

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

    setAcceptDrops(true);

    connect(ui->actionNew, &QAction::triggered, this, &PianoRollWidget::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, this, &PianoRollWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &PianoRollWidget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionCopy, &QAction::triggered, this, &PianoRollWidget::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &PianoRollWidget::paste);
    connect(ui->actionSelectAll, &QAction::triggered, this, &PianoRollWidget::selectAll);
    connect(ui->actionDelete, &QAction::triggered, this, &PianoRollWidget::deleteTriggered);

    connect(&_velocityAction, &QAction::triggered, this, &PianoRollWidget::velocityTriggered);

    connect(ui->ganttWidget, &GanttWidget::headerClicked, this, &PianoRollWidget::ganttHeaderClicked);
    connect(ui->ganttWidget, &GanttWidget::markerClicked, this, &PianoRollWidget::ganttMarkerClicked);
    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PianoRollWidget::ganttEditorClicked);
    connect(ui->ganttWidget, &GanttWidget::itemChanged, this, &PianoRollWidget::ganttItemChanged);
    connect(ui->ganttWidget, &GanttWidget::itemReleased, this, &PianoRollWidget::ganttItemReleased);
    connect(ui->ganttWidget, &GanttWidget::contextMenuRequested, this, &PianoRollWidget::contextMenuRequested);

    connect(ui->fmDoneButton, &QPushButton::pressed, this, &PianoRollWidget::doneButtonClicked);
    connect(ui->noiseDoneButton, &QPushButton::pressed, this, &PianoRollWidget::doneButtonClicked);
    connect(ui->fmRemoveButton, &QPushButton::pressed, this, &PianoRollWidget::removeButtonClicked);
    connect(ui->noiseRemoveButton, &QPushButton::pressed, this, &PianoRollWidget::removeButtonClicked);

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
    _pattern = &_app->project().getPattern(pattern);
    _channel = track;
    _track = &_app->project().getPattern(pattern).getTrack(track);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&_track->items()));
    ui->ganttWidget->setMarkers(reinterpret_cast<QList<GanttMarker*>*>(&_track->settingsChanges()));
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

Pattern& PianoRollWidget::pattern() const
{
    return *_pattern;
}

void PianoRollWidget::pressKey(const int key)
{
    _keysWidget->pressKey(key);
}

void PianoRollWidget::releaseKey(const int key)
{
    _keysWidget->releaseKey(key);
}

bool PianoRollWidget::hasLoop() const
{
    return ui->ganttWidget->hasLoop();
}

float PianoRollWidget::loopStart() const
{
    return ui->ganttWidget->loopStart();
}

float PianoRollWidget::loopEnd() const
{
    return ui->ganttWidget->loopEnd();
}

void PianoRollWidget::doUpdate()
{
    if (_app->project().getChannel(_channel).type() == Channel::Type::FM) {
        ui->fmWidget->doUpdate();
    } else if (_app->project().getChannel(_channel).type() == Channel::Type::NOISE) {
        ui->noiseWidget->doUpdate();
    }
}

void PianoRollWidget::loadMIDI(const MIDIFile& file)
{
    auto it1 = std::find_if(file.chunks().begin(), file.chunks().end(), [](const MIDIChunk* chunk){ return dynamic_cast<const MIDIHeader*>(chunk); });

    if (it1 != file.chunks().end()) {
        const MIDIHeader* header = dynamic_cast<const MIDIHeader*>(*it1);

        auto it2 = std::find_if(file.chunks().begin(), file.chunks().end(), [](const MIDIChunk* chunk){ return dynamic_cast<const MIDITrack*>(chunk); });

        if (it2 != file.chunks().end()) {
            const MIDITrack* track = dynamic_cast<const MIDITrack*>(*it2);

            QList<Track::Item*> items = MIDI::toTrackItems(*track, header->division());

            _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), *_track, items));
        }
    }
}

void PianoRollWidget::ganttMarkerClicked(GanttMarker* marker)
{
    Track::SettingsChange* settingsChange = dynamic_cast<Track::SettingsChange*>(marker);

    if (_app->project().getChannel(_channel).type() == Channel::Type::FM) {
        ui->fmWidget->setSettings(&dynamic_cast<FMChannelSettings&>(settingsChange->settings()));
        ui->stackedWidget->setCurrentIndex(1);
        _editingSettingsChange = dynamic_cast<Track::SettingsChange*>(marker);
    } else if (_app->project().getChannel(_channel).type() == Channel::Type::NOISE) {
        ui->noiseWidget->setSettings(&dynamic_cast<NoiseChannelSettings&>(settingsChange->settings()));
        ui->stackedWidget->setCurrentIndex(2);
        _editingSettingsChange = dynamic_cast<Track::SettingsChange*>(marker);
    }
}

void PianoRollWidget::ganttHeaderClicked(Qt::MouseButton button, float time)
{
    if (button == Qt::LeftButton) {
        if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
            if (_app->project().getChannel(_channel).type() == Channel::Type::NOISE) {
                NoiseChannelSettings& ncs = dynamic_cast<NoiseChannelSettings&>(_app->project().getChannel(_channel).settings());
                _app->undoStack().push(new AddTrackSettingsChangeCommand(_app->window(), *_track, time, new NoiseChannelSettings(ncs)));
            } else if (_app->project().getChannel(_channel).type() == Channel::Type::FM) {
                FMChannelSettings& fmcs = dynamic_cast<FMChannelSettings&>(_app->project().getChannel(_channel).settings());
                _app->undoStack().push(new AddTrackSettingsChangeCommand(_app->window(), *_track, time, new FMChannelSettings(fmcs)));
            }
        } else if (_app->project().playMode() == Project::PlayMode::PATTERN) {
            _app->setPosition(time);
        }
    }
}

void PianoRollWidget::ganttEditorClicked(Qt::MouseButton button, int row, float time)
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

    const QList<Track::Item*>& selectedItems = reinterpret_cast<const QList<Track::Item*>&>(ui->ganttWidget->selectedItems());

    for (Track::Item* item : selectedItems) {
        Note n = item->note();
        n.setVelocity(velocity);

        _app->undoStack().push(new EditNoteCommand(_app->window(), item, item->time(), n, selectedItems));
    }
}

void PianoRollWidget::newTriggered()
{
    _app->undoStack().push(new RemoveTrackItemsCommand(_app->window(), *_track, _track->items()));
}

void PianoRollWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(nullptr, tr("Open file"), "", "MIDI File (*.mid)");

    if (!path.isNull()) {
        QFile file(path);
        MIDIFile mfile(file);

        loadMIDI(mfile);
    }
}

void PianoRollWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(nullptr, tr("Save file"), "", "MIDI File (*.mid)");

    if (!path.isNull()) {
        MIDIFile midiFile;
        MIDI::fromTrack(*_track, midiFile, 480);

        QByteArray data = midiFile.encode();

        QFile file(path);
        file.open(QIODevice::WriteOnly);
        QDataStream stream(&file);

        stream.writeRawData(data.constData(), data.size());

        file.close();
    }
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

void PianoRollWidget::doneButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void PianoRollWidget::removeButtonClicked()
{
    _app->undoStack().push(new RemoveTrackSettingsChangeCommand(_app->window(), *_track, _editingSettingsChange));
    ui->stackedWidget->setCurrentIndex(0);
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

void PianoRollWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void PianoRollWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.replace("\r\n", "");

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "mid") {
        MIDIFile mfile(file);

        loadMIDI(mfile);

        event->acceptProposedAction();
    }
}

