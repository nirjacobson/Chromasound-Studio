#include "pianorollwidget.h"
#include "ui_pianorollwidget.h"

PianoRollWidget::PianoRollWidget(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , _app(app)
    , ui(new Ui::PianoRollWidget)
    , _keysWidget(new PianoRollKeysWidget(this, app))
    , _velocitiesWidget(new PianoRollVelocitiesWidget(this))
    , _itemLastClicked(nullptr)
    , _noteMenu(tr("Context menu"), this)
    , _velocityAction("Velocity", this)
    , _editingSettingsChange(nullptr)
    , _velocityDialog(new PianoRollVelocityDialog(this))
{
    _velocitiesWidget->setApplication(app);

    ui->setupUi(this);

    ui->settingsChangeWidget->setApplication(app);

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
    connect(ui->actionQuantizeKeyOn, &QAction::triggered, this, &PianoRollWidget::quantizeKeyOn);
    connect(ui->actionQuantizeKeyOnAndDuration, &QAction::triggered, this, &PianoRollWidget::quantizeKeyOnAndDuration);

    connect(&_velocityAction, &QAction::triggered, this, &PianoRollWidget::velocityTriggered);

    connect(ui->ganttWidget, &GanttWidget::headerClicked, this, &PianoRollWidget::ganttHeaderClicked);
    connect(ui->ganttWidget, &GanttWidget::markerClicked, this, &PianoRollWidget::ganttMarkerClicked);
    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PianoRollWidget::ganttEditorClicked);
    connect(ui->ganttWidget, &GanttWidget::itemChanged, this, &PianoRollWidget::ganttItemChanged);
    connect(ui->ganttWidget, &GanttWidget::itemReleased, this, &PianoRollWidget::ganttItemReleased);
    connect(ui->ganttWidget, &GanttWidget::contextMenuRequested, this, &PianoRollWidget::contextMenuRequested);

    connect(_keysWidget, &PianoRollKeysWidget::keyOn, this, &PianoRollWidget::keyOn);
    connect(_keysWidget, &PianoRollKeysWidget::keyOff, this, &PianoRollWidget::keyOff);

    connect(ui->settingsChangeWidget, &SettingsChangeWidget::keyOn, this, &PianoRollWidget::keyOn);
    connect(ui->settingsChangeWidget, &SettingsChangeWidget::keyOff, this, &PianoRollWidget::keyOff);
    connect(ui->settingsChangeWidget, &SettingsChangeWidget::removeClicked, this, &PianoRollWidget::removeButtonClicked);
    connect(ui->settingsChangeWidget, &SettingsChangeWidget::doneClicked, this, &PianoRollWidget::doneButtonClicked);

    connect(_velocityDialog, &QDialog::accepted, this, &PianoRollWidget::velocityDialogAccepted);

    ui->settingsChangeWidget->setVisible(false);

    ui->menubar->setNativeMenuBar(false);
}

PianoRollWidget::~PianoRollWidget()
{
    delete ui;

    delete _velocityDialog;
    delete _velocitiesWidget;
    delete _keysWidget;
}

void PianoRollWidget::setTrack(const int pattern, const int track)
{
    _pattern = &_app->project().getPattern(pattern);
    _channel = track;
    _track = &_app->project().getPattern(pattern).getTrack(track);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&_track->items()));
    ui->ganttWidget->addMarkers(reinterpret_cast<QList<GanttMarker*>*>(&_track->settingsChanges()));
    ui->ganttWidget->setPositionFunction([&]() {
        if (_app->project().playMode() == Project::PlayMode::SONG) {
            QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(_appPosition);

            if (activePatterns.contains(pattern)) {
                float delta = _appPosition - activePatterns[pattern];

                return delta;
            }
        } else {
            return _appPosition;
        }

        return -1.0f;
    });

    try {
        const PCMChannelSettings& settings = dynamic_cast<const PCMChannelSettings&>(_app->project().getChannel(_channel).settings());
        _keysWidget->setROMChannelSettings(&settings);
    } catch (std::exception& e) { }

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
    ui->settingsChangeWidget->pressKey(key);
}

void PianoRollWidget::releaseKey(const int key)
{
    _keysWidget->releaseKey(key);
    ui->settingsChangeWidget->releaseKey(key);
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

void PianoRollWidget::doUpdate(const float position, bool full)
{
    _appPosition = position;
    ui->ganttWidget->doUpdate(full);
    ui->settingsChangeWidget->doUpdate();
}

void PianoRollWidget::setCellMajors(const QList<int>& majors)
{
    ui->ganttWidget->setCellMajors(majors);
}

ChannelSettings& PianoRollWidget::currentSettings()
{
    if (_editingSettingsChange) {
        return _editingSettingsChange->settings();
    } else {
        return _app->project().getChannel(_channel).settings();
    }
}

void PianoRollWidget::loadMIDI(const MIDIFile& file)
{
    auto it1 = std::find_if(file.chunks().begin(), file.chunks().end(), [](const MIDIChunk* chunk) {
        return dynamic_cast<const MIDIHeader*>(chunk);
    });

    if (it1 != file.chunks().end()) {
        const MIDIHeader* header = dynamic_cast<const MIDIHeader*>(*it1);

        auto it2 = std::find_if(file.chunks().begin(), file.chunks().end(), [](const MIDIChunk* chunk) {
            return dynamic_cast<const MIDITrack*>(chunk);
        });

        if (it2 != file.chunks().end()) {
            const MIDITrack* track = dynamic_cast<const MIDITrack*>(*it2);

            QList<Track::Item*> items = MIDI::toTrackItems(*track, header->division());

            _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), *_track, items));
        }
    }
}

const QColor& PianoRollWidget::settingsChangeColor() const
{
    return Track::SettingsChange::COLOR;
}

void PianoRollWidget::setSettingsChangeColor(const QColor& color)
{
    Track::SettingsChange::COLOR = color;
}

void PianoRollWidget::ganttMarkerClicked(GanttMarker* marker)
{
    Track::SettingsChange* settingsChange = dynamic_cast<Track::SettingsChange*>(marker);

    ui->settingsChangeWidget->setSettings(settingsChange->settings());
    ui->settingsChangeWidget->setVisible(true);
    ui->stackedWidget->setCurrentIndex(1);

    _editingSettingsChange = settingsChange;

    emit sizeUpNeeded();
}

void PianoRollWidget::ganttHeaderClicked(Qt::MouseButton button, float time)
{
    if (button == Qt::LeftButton) {
        if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
            ChannelSettings& cs = _app->project().getChannel(_channel).settings();
            _app->undoStack().push(new AddTrackSettingsChangeCommand(_app->window(), *_track, time, &cs));
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
    _velocityDialog->setOperation(PianoRollVelocityDialog::Set);
    _velocityDialog->setValue(_contextItem->velocity());
    _velocityDialog->exec();
}

void PianoRollWidget::newTriggered()
{
    _app->undoStack().push(new RemoveTrackItemsCommand(_app->window(), *_track, _track->items()));
}

void PianoRollWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "MIDI File (*.mid)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        MIDIFile mfile(file);

        loadMIDI(mfile);
    }
}

void PianoRollWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "MIDI File (*.mid)", nullptr, QFileDialog::DontUseNativeDialog);

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
    ui->settingsChangeWidget->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    _editingSettingsChange = nullptr;

    emit sizeDownNeeded();
}

void PianoRollWidget::removeButtonClicked()
{
    _app->undoStack().push(new RemoveTrackSettingsChangeCommand(_app->window(), *_track, _editingSettingsChange));
    ui->settingsChangeWidget->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    _editingSettingsChange = nullptr;

    emit sizeDownNeeded();
}

void PianoRollWidget::velocityDialogAccepted()
{
    QList<Track::Item*> selectedItems = reinterpret_cast<const QList<Track::Item*>&>(ui->ganttWidget->selectedItems());

    if (!selectedItems.contains(_contextItem)) {
        selectedItems.append(_contextItem);
    }

    for (Track::Item* item : selectedItems) {
        Note n = item->note();

        switch (_velocityDialog->operation()) {
        case PianoRollVelocityDialog::Set:
            n.setVelocity(_velocityDialog->value());
            break;
        case PianoRollVelocityDialog::Scale:
            n.setVelocity(_velocityDialog->doubleValue() * n.velocity());
            break;
        case PianoRollVelocityDialog::Add:
            n.setVelocity(_velocityDialog->value() + n.velocity());
            break;
        }

        n.setVelocity(qMax(0, n.velocity()));
        n.setVelocity(qMin(100, n.velocity()));

        _app->undoStack().push(new EditNoteCommand(_app->window(), item, item->time(), n, selectedItems));
    }
}

void PianoRollWidget::quantizeKeyOn()
{
    QList<Track::Item*> newItems;

    if (!ui->ganttWidget->selectedItems().empty()) {
        QList<Track::Item*> selectedItems;
        for (GanttItem* item : ui->ganttWidget->selectedItems()) {
            Track::Item* trackItem = dynamic_cast<Track::Item*>(item);
            selectedItems.append(trackItem);

            float time = std::round(item->time() / 0.25) * 0.25;

            Track::Item* newItem = new Track::Item(time, Note(trackItem->note().key(), trackItem->note().duration(), trackItem->note().velocity()));

            newItems.append(newItem);
        }

        _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), *_track, selectedItems, newItems));
    } else {
        for (Track::Item* item : _track->items()) {
            float time = std::round(item->time() / 0.25) * 0.25;

            Track::Item* newItem = new Track::Item(time, Note(item->note().key(), item->note().duration(), item->note().velocity()));

            newItems.append(newItem);
        }

        _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), *_track, newItems));
    }
}

void PianoRollWidget::quantizeKeyOnAndDuration()
{
    QList<Track::Item*> newItems;

    if (!ui->ganttWidget->selectedItems().empty()) {
        QList<Track::Item*> selectedItems;
        for (GanttItem* item : ui->ganttWidget->selectedItems()) {
            Track::Item* trackItem = dynamic_cast<Track::Item*>(item);
            selectedItems.append(trackItem);

            float time = std::round(item->time() / 0.25) * 0.25;
            float duration = std::round(item->duration() / 0.25) * 0.25;

            Track::Item* newItem = new Track::Item(time, Note(trackItem->note().key(), duration, trackItem->note().velocity()));

            newItems.append(newItem);
        }

        _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), *_track, selectedItems, newItems));
    } else {
        for (Track::Item* item : _track->items()) {
            float time = std::round(item->time() / 0.25) * 0.25;
            float duration = std::round(item->duration() / 0.25) * 0.25;

            Track::Item* newItem = new Track::Item(time, Note(item->note().key(), duration, item->note().velocity()));

            newItems.append(newItem);
        }

        _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), *_track, newItems));
    }
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
    path = path.split("\r\n").at(0);

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "mid") {
        MIDIFile mfile(file);

        loadMIDI(mfile);

        event->acceptProposedAction();
    }
}

