#include "channelswidget.h"
#include "ui_channelswidget.h"

ChannelsWidget::ChannelsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::ChannelsWidget),
    _app(app),
    _activeChannelWidget(nullptr),
    _stepCursorWidget(nullptr)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    rebuild();

    connect(ui->pianoButton, &QPushButton::clicked, this, &ChannelsWidget::pianoButtonClicked);
    connect(ui->velocityButton, &QPushButton::clicked, this, &ChannelsWidget::velocityButtonClicked);
    connect(ui->addButton, &QPushButton::clicked, this, &ChannelsWidget::channelAdded);
}

ChannelsWidget::~ChannelsWidget()
{
    for (ChannelWidget* channelWidget : _channelWidgets) {
        delete channelWidget;
    }

    delete ui;
}

void ChannelsWidget::rebuild()
{
    QList<QLayoutItem*> items;
    for (int i = 0; i < ui->verticalLayout->count(); i++) {
        items.append(ui->verticalLayout->itemAt(i));
    }
    for (QLayoutItem* item : items) {
        ui->verticalLayout->removeItem(item);
        delete item->widget();
        delete item;
    }

    _channelWidgets.clear();

    for (int i = 0; i < _app->project().channelCount(); i++) {
        _channelWidgets.append(new ChannelWidget(this, _app, i));
    }

    for (ChannelWidget* channelWidget : _channelWidgets) {
        ui->verticalLayout->addWidget(channelWidget);
        connect(channelWidget, &ChannelWidget::ledShiftClicked, this, [=]() {
            toggleSolo(channelWidget);
        });
        connect(channelWidget, &ChannelWidget::toggled, this, [=](bool selected) {
            handleToggle(channelWidget, selected);
        });
        connect(channelWidget, &ChannelWidget::rectLedClicked, this, [=]() {
            handleSelect(channelWidget);
        });
        connect(channelWidget, &ChannelWidget::rectLedDoubleClicked, this, &ChannelsWidget::handleSelectAll);
        connect(channelWidget, &ChannelWidget::pianoRollTriggered, this, [=](bool on) {
            emit pianoRollTriggered(channelWidget->index(), on);
        });
        connect(channelWidget, &ChannelWidget::selected, this, [=]() {
            select(channelWidget->index());
        });
        connect(channelWidget, &ChannelWidget::nameChanged, this, [=]() {
            emit nameChanged(channelWidget->index());
        });
        connect(channelWidget, &ChannelWidget::pianoKeyClicked, this, &ChannelsWidget::pianoKeyClicked);
        connect(channelWidget, &ChannelWidget::velocityClicked, this, &ChannelsWidget::velocityClicked);
    }

    if (!_channelWidgets.isEmpty()) {
        _stepCursorWidget = new StepCursorWidget(this, _app);
        QWidget* widget = new QWidget();
        widget->setLayout(new QHBoxLayout);
        widget->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
        widget->layout()->addWidget(_stepCursorWidget);
        ui->verticalLayout->addWidget(widget);
    }
}

void ChannelsWidget::remove(const int index)
{
    QLayoutItem* item = ui->verticalLayout->itemAt(index);
    ui->verticalLayout->removeItem(item);
    item->widget()->deleteLater();
    delete item;
    _channelWidgets.removeAt(index);

    for (int i = index; i < _channelWidgets.size(); i++) {
        _channelWidgets[i]->setIndex(_channelWidgets[i]->index() - 1);
    }
}

void ChannelsWidget::add(const int index)
{
    ChannelWidget* cw = new ChannelWidget(this, _app, index);
    connect(cw, &ChannelWidget::ledShiftClicked, this, [=]() {
        toggleSolo(cw);
    });
    connect(cw, &ChannelWidget::toggled, this, [=](bool selected) {
        handleToggle(cw, selected);
    });
    connect(cw, &ChannelWidget::rectLedClicked, this, [=]() {
        handleSelect(cw);
    });
    connect(cw, &ChannelWidget::rectLedDoubleClicked, this, &ChannelsWidget::handleSelectAll);
    connect(cw, &ChannelWidget::pianoRollTriggered, this, [=](bool on) {
        emit pianoRollTriggered(cw->index(), on);
    });
    connect(cw, &ChannelWidget::selected, this, [=]() {
        select(cw->index());
    });
    connect(cw, &ChannelWidget::nameChanged, this, [=]() {
        emit nameChanged(cw->index());
    });
    connect(cw, &ChannelWidget::pianoKeyClicked, this, &ChannelsWidget::pianoKeyClicked);
    connect(cw, &ChannelWidget::velocityClicked, this, &ChannelsWidget::velocityClicked);
    ui->verticalLayout->insertWidget(index, cw);

    _channelWidgets.insert(index, cw);

    for (int i = index + 1; i < _channelWidgets.size(); i++) {
        _channelWidgets[i]->setIndex(i);
    }
}

void ChannelsWidget::setVolume(const int index, const int volume)
{
    _channelWidgets[index]->setVolume(volume);
}

void ChannelsWidget::doUpdate(const float position)
{
    for (ChannelWidget* channelWidget : _channelWidgets) {
        channelWidget->doUpdate(position);
    }
    if (_stepCursorWidget) {
        _stepCursorWidget->doUpdate(position);
    }
}

void ChannelsWidget::update(const int index)
{
    _channelWidgets[index]->setIndex(index);
}

void ChannelsWidget::update()
{
    QWidget::update();
}

void ChannelsWidget::select(const int index)
{
    _channelWidgets[index]->blockSignals(true);
    _channelWidgets[index]->setChecked(true);
    _channelWidgets[index]->blockSignals(false);

    handleToggle(_channelWidgets[index], true);
}

int ChannelsWidget::selected() const
{
    for (int i = 0; i < _channelWidgets.size(); i++) {
        if (_channelWidgets[i]->checked()) {
            return i;
        }
    }

    return -1;
}

int ChannelsWidget::activeChannel() const
{
    if (!_activeChannelWidget) return -1;

    return _activeChannelWidget->index();
}

void ChannelsWidget::toggleSolo(ChannelWidget* channelWidget)
{
    bool on = channelWidget->on();

    for (ChannelWidget* cw : _channelWidgets) {
        cw->setOn(cw == channelWidget ? on : !on);
    }
}

void ChannelsWidget::handleToggle(ChannelWidget* channelWidget, const bool selected)
{
    for (ChannelWidget* cw : _channelWidgets) {
        if (cw != channelWidget) {
            cw->blockSignals(true);
            cw->setChecked(false);
            cw->blockSignals(false);
        }
    }

    if (_activeChannelWidget) {
        _activeChannelWidget->hideStepWidgets();
    }

    _activeChannelWidget = nullptr;

    if (selected) {
        _activeChannelWidget = channelWidget;

        const Pattern& pattern = _app->project().getFrontPattern();
        if (pattern.hasTrack(_activeChannelWidget->index())) {
            const Track& track = pattern.getTrack(_activeChannelWidget->index());
            if (!track.doesUsePianoRoll()) {
                if (ui->pianoButton->isChecked()) {
                    _activeChannelWidget->showStepKeysWidget();
                } else if (ui->velocityButton->isChecked() && !track.items().empty()) {
                    _activeChannelWidget->showStepVelsWidget();
                }
            }
        } else {
            if (ui->pianoButton->isChecked()) {
                _activeChannelWidget->showStepKeysWidget();
            }
        }

        emit channelSelected(channelWidget->index());
    }
}

void ChannelsWidget::handleSelect(ChannelWidget* channelWidget)
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setSelected(cw == channelWidget);
    }
}

void ChannelsWidget::handleSelectAll()
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setSelected(true);
    }
}

void ChannelsWidget::pianoButtonClicked()
{
    if (_activeChannelWidget) {
        _activeChannelWidget->hideStepWidgets();
    }

    if (ui->pianoButton->isChecked()) {
        ui->velocityButton->setChecked(false);

        if (_activeChannelWidget) {
            const Pattern& pattern = _app->project().getFrontPattern();
            if (pattern.hasTrack(_activeChannelWidget->index())) {
                const Track& track = pattern.getTrack(_activeChannelWidget->index());
                if(!track.doesUsePianoRoll()) {
                    _activeChannelWidget->showStepKeysWidget();
                }
            } else {
                _activeChannelWidget->showStepKeysWidget();
            }
        }
    }
}

void ChannelsWidget::velocityButtonClicked()
{
    if (_activeChannelWidget) {
        _activeChannelWidget->hideStepWidgets();
    }

    if (ui->velocityButton->isChecked()) {
        ui->pianoButton->setChecked(false);

        if (_activeChannelWidget) {
            const Pattern& pattern = _app->project().getFrontPattern();
            if (pattern.hasTrack(_activeChannelWidget->index())) {
                const Track& track = pattern.getTrack(_activeChannelWidget->index());
                if (!track.doesUsePianoRoll() && !track.items().empty()) {
                    _activeChannelWidget->showStepVelsWidget();
                }
            }
        }
    }
}

void ChannelsWidget::pianoKeyClicked(const Qt::MouseButton button, const int step, const int key)
{
    float beatsPerStep = 0.25;
    Track& track = _app->project().getFrontPattern().getTrack(_activeChannelWidget->index());

    if (button == Qt::LeftButton) {
        auto existingItem = std::find_if(track.items().begin(), track.items().end(), [=](const Track::Item* item) {
            return item->time() == step * beatsPerStep;
        });
        if (existingItem == track.items().end()) {
            _app->undoStack().push(new AddNoteCommand(_app->window(), track, step * beatsPerStep, Note(key, beatsPerStep)));
        } else {
            _app->undoStack().push(new EditNoteCommand(_app->window(), *existingItem, step * beatsPerStep, Note(key, beatsPerStep), QList<Track::Item*>()));
        }
    } else {
        _app->undoStack().push(new RemoveNoteCommand(_app->window(), track, step * beatsPerStep, Note(key, beatsPerStep)));
    }

}

void ChannelsWidget::velocityClicked(const int step, const int velocity)
{
    float beatsPerStep = 0.25;
    Track& track = _app->project().getFrontPattern().getTrack(_activeChannelWidget->index());

    auto existingItem = std::find_if(track.items().begin(), track.items().end(), [=](const Track::Item* item) {
        return item->time() == step * beatsPerStep;
    });
    if (existingItem != track.items().end()) {
        Note newNote = (*existingItem)->note();
        newNote.setVelocity(velocity);
        _app->undoStack().push(new EditNoteCommand(_app->window(), *existingItem, step * beatsPerStep, newNote, QList<Track::Item*>()));
    }
}

void ChannelsWidget::channelAdded()
{
    _app->undoStack().push(new AddChannelCommand(_app->window()));
}

void ChannelsWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChannelsWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void ChannelsWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString pathsString(data);
    QStringList paths = pathsString.split("\r\n");
    paths.removeDuplicates();
    paths.removeAll("");

    for (QString& str : paths) {
        str = str.mid(QString("file://").length());
        str = str.replace("%20", " ");
    }

    for (const QString& path : paths) {
        QFile file(path);
        QFileInfo fileInfo(file);

        int index = _app->project().channelCount();

        if (fileInfo.suffix() == "opn") {
            FMChannelSettings* settings = BSON::decodeFM4Patch(path);
            _app->undoStack().push(new AddFMChannelCommand(_app->window(), *settings, QFileInfo(QFile(path)).baseName()));

            delete settings;
        } else if (fileInfo.suffix() == "pcm") {
            PCMChannelSettings* settings = new PCMChannelSettings;
            settings->setPath(path);
            _app->undoStack().push(new AddPCMChannelCommand(_app->window(), *settings, QFileInfo(QFile(path)).baseName()));

            delete settings;
        } else if (fileInfo.suffix() == "mid") {
            MIDIFile mfile(file);

            auto it1 = std::find_if(mfile.chunks().begin(), mfile.chunks().end(), [](const MIDIChunk* chunk) {
                return dynamic_cast<const MIDIHeader*>(chunk);
            });

            if (it1 != mfile.chunks().end()) {
                const MIDIHeader* header = dynamic_cast<const MIDIHeader*>(*it1);

                auto it2 = std::find_if(mfile.chunks().begin(), mfile.chunks().end(), [](const MIDIChunk* chunk) {
                    return dynamic_cast<const MIDITrack*>(chunk);
                });

                if (it2 != mfile.chunks().end()) {
                    const MIDITrack* track = dynamic_cast<const MIDITrack*>(*it2);

                    QList<Track::Item*> items = MIDI::toTrackItems(*track, header->division());

                    _app->undoStack().push(new AddTrackCommand(_app->window(), items, QFileInfo(file).baseName()));
                }
            }
        }
    }
}



