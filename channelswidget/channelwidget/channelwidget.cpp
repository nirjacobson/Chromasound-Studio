#include "channelwidget.h"
#include "ui_channelwidget.h"
#include <QDial>

ChannelWidget::ChannelWidget(QWidget *parent, Application* app, int index)
    : QWidget(parent)
    , ui(new Ui::ChannelWidget)
    , _app(app)
    , _index(index)
    , _contextMenu(tr("Context menu"), this)
    , _pianoRollAction("Piano roll", this)
    , _moveUpAction("Move up", this)
    , _moveDownAction("Move down", this)
    , _deleteAction("Delete", this)
    , _typeActionGroup(this)
    , _toneAction("Tone", this)
    , _noiseAction("Noise", this)
    , _fmAction("FM", this)
    , _pcmAction("PCM", this)
    , _fillEvery2StepsAction("Every 2 steps")
    , _fillEvery4StepsAction("Every 4 steps")
    , _toneColor(Qt::cyan)
    , _noiseColor(Qt::lightGray)
    , _fmColor(Qt::magenta)
    , _pcmColor(Qt::blue)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    switch (_app->project().getChannel(_index).type()) {
        case Channel::TONE:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_toneColor.name()));
            break;
        case Channel::NOISE:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_noiseColor.name()));
            break;
        case Channel::FM:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_fmColor.name()));
            break;
        case Channel::PCM:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_pcmColor.name()));
            break;
    }

    ui->trackStackedWidget->setMinimumHeight(32);
    ui->stepsStackedWidget->setMinimumHeight(128);

    ui->stepKeys->setApplication(app);
    ui->stepVelocities->setApplication(app);
    ui->stepKeys->setChannel(index);
    ui->stepVelocities->setChannel(index);

    ui->stepsStackedWidget->setVisible(false);

    ui->stepSequencer->setApplication(app);
    ui->prDisplay->setApplication(app);

    setIndex(index);

    connect(ui->pushButton, &QPushButton::pressed, this, &ChannelWidget::buttonPressed);
    connect(ui->pushButton, &QWidget::customContextMenuRequested, this, &ChannelWidget::buttonContextMenuRequested);
    connect(ui->led, &LED::clicked, this, &ChannelWidget::ledClicked);

    connect(ui->volumeDial, &QDial::valueChanged, this, &ChannelWidget::volumeDialChanged);

    connect(ui->rectLed, &RectLED::clicked, this, &ChannelWidget::rectLedClicked);
    connect(ui->rectLed, &RectLED::doubleClicked, this, &ChannelWidget::rectLedDoubleClicked);

    connect(ui->prDisplay, &PRDisplayWidget::pianoRollTriggered, this, &ChannelWidget::pianoRollWasTriggered);

    connect(&_pianoRollAction, &QAction::triggered, this, &ChannelWidget::pianoRollWasToggled);
    connect(&_moveUpAction, &QAction::triggered, this, &ChannelWidget::moveUpTriggered);
    connect(&_moveDownAction, &QAction::triggered, this, &ChannelWidget::moveDownTriggered);
    connect(&_deleteAction, &QAction::triggered, this, &ChannelWidget::deleteTriggered);

    connect(&_fillEvery2StepsAction, &QAction::triggered, this, &ChannelWidget::fillEvery2StepsTriggered);
    connect(&_fillEvery4StepsAction, &QAction::triggered, this, &ChannelWidget::fillEvery4StepsTriggered);

    connect(&_toneAction, &QAction::triggered, this, &ChannelWidget::toneWasTriggered);
    connect(&_noiseAction, &QAction::triggered, this, &ChannelWidget::noiseWasTriggered);
    connect(&_fmAction, &QAction::triggered, this, &ChannelWidget::fmWasTriggered);
    connect(&_pcmAction, &QAction::triggered, this, &ChannelWidget::pcmWasTriggered);

    connect(ui->stepKeys, &StepKeysWidget::clicked, this, &ChannelWidget::pianoKeyClicked);
    connect(ui->stepVelocities, &StepVelocitiesWidget::clicked, this, &ChannelWidget::velocityClicked);

    const Pattern& frontPattern = _app->project().getFrontPattern();
    _pianoRollAction.setCheckable(true);
    _pianoRollAction.setChecked(frontPattern.hasTrack(index) && frontPattern.getTrack(index).doesUsePianoRoll());
    _contextMenu.addAction(&_pianoRollAction);

    _contextMenu.addSeparator();
    _contextMenu.addAction(&_moveUpAction);
    _contextMenu.addAction(&_moveDownAction);

    _contextMenu.addSection("Fill");
    _contextMenu.addAction(&_fillEvery2StepsAction);
    _contextMenu.addAction(&_fillEvery4StepsAction);

    _toneAction.setCheckable(true);
    _toneAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::TONE);
    _noiseAction.setCheckable(true);
    _noiseAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::NOISE);
    _fmAction.setCheckable(true);
    _fmAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::FM);
    _pcmAction.setCheckable(true);
    _pcmAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::PCM);
    _typeActionGroup.addAction(&_toneAction);
    _typeActionGroup.addAction(&_noiseAction);
    _typeActionGroup.addAction(&_fmAction);
    _typeActionGroup.addAction(&_pcmAction);

    _contextMenu.addSection("Type");
    _contextMenu.addAction(&_toneAction);
    _contextMenu.addAction(&_noiseAction);
    _contextMenu.addAction(&_fmAction);
    _contextMenu.addAction(&_pcmAction);

    _contextMenu.addSeparator();
    _contextMenu.addAction(&_deleteAction);
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::setOn(const bool on)
{
    Channel& channel = _app->project().getChannel(_index);
    channel.setEnabled(on);

    ui->led->setOn(on);
}

void ChannelWidget::setChecked(const bool checked)
{
    ui->pushButton->setChecked(checked);
}

void ChannelWidget::setSelected(const bool selected)
{
    ui->rectLed->setSelected(selected);
}

bool ChannelWidget::on() const
{
    return _app->project().getChannel(_index).enabled();
}

bool ChannelWidget::checked() const
{
    return ui->pushButton->isChecked();
}

int ChannelWidget::index() const
{
    return _index;
}

void ChannelWidget::setIndex(const int idx)
{
    _index = idx;

    switch (_app->project().getChannel(_index).type()) {
        case Channel::TONE:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_toneColor.name()));
            break;
        case Channel::NOISE:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_noiseColor.name()));
            break;
        case Channel::FM:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_fmColor.name()));
            break;
        case Channel::PCM:
            ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_pcmColor.name()));
            break;
    }

    ui->pushButton->setText(_app->project().getChannel(_index).name());
    ui->stepSequencer->setIndex(_index);
    ui->prDisplay->setIndex(_index);
    ui->led->setOn(_app->project().getChannel(_index).enabled());
    ui->volumeDial->setValue(_app->project().getChannel(_index).settings().volume());
    ui->rectLed->setOnFunction([=]() {
        float appPosition = _app->position();

        if (_app->project().playMode() == Project::PlayMode::PATTERN) {
            Pattern& pat = _app->project().getFrontPattern();

            if (pat.activeTracksAtTime(appPosition).contains(_index)) {
                const Track& track = pat.getTrack(_index);
                return _app->project().getChannel(_index).enabled() &&
                       std::find_if(track.items().begin(),
                                    track.items().end(),
                                    [&](const Track::Item* item) {
                                        float delta = item->time() - appPosition;
                                        return qAbs(delta) <= 0.0625;
                                    }) == track.items().end();
            }
        } else {
            QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(appPosition);
            for (auto it = activePatterns.begin(); it != activePatterns.end(); ++it) {
                float _appPosition = appPosition;
                Pattern& pat = _app->project().getPattern(it.key());
                _appPosition -= it.value();

                if (pat.activeTracksAtTime(_appPosition).contains(_index)) {
                    const Track& track = pat.getTrack(_index);
                    return _app->project().getChannel(_index).enabled() &&
                           std::find_if(track.items().begin(),
                                        track.items().end(),
                                        [&](const Track::Item* item) {
                                            float delta = item->time() - _appPosition;
                                            return qAbs(delta) <= 0.0625;
                                        }) == track.items().end();
                }
            }
        }
        return false;

    });

    ui->stepKeys->setChannel(_index);
    ui->stepVelocities->setChannel(_index);

    _noiseAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::NOISE);
    _toneAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::TONE);
    _fmAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::FM);
    _pcmAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::PCM);

    update();
}

void ChannelWidget::setVolume(const int volume)
{
    _app->project().getChannel(_index).settings().setVolume(volume);
    ui->volumeDial->blockSignals(true);
    ui->volumeDial->setValue(volume);
    ui->volumeDial->blockSignals(false);
}

void ChannelWidget::showStepKeysWidget()
{
    ui->stepsStackedWidget->setCurrentIndex(0);
    ui->stepsStackedWidget->setVisible(true);
}

void ChannelWidget::showStepVelsWidget()
{
    ui->stepsStackedWidget->setCurrentIndex(1);
    ui->stepsStackedWidget->setVisible(true);
}

void ChannelWidget::hideStepWidgets()
{
    ui->stepsStackedWidget->setVisible(false);
}

const QRect ChannelWidget::getSequencerGeometry()
{
    QRect rect;
    rect.setBottomLeft(ui->stepSequencer->mapToGlobal(ui->stepSequencer->geometry().bottomLeft()));
    rect.setWidth(ui->stepSequencer->width());
    return rect;
}

void ChannelWidget::fromPath(const QString& path)
{
    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "fm") {
        FMChannelSettings* settings = BSON::decodePatch(path);
        _app->undoStack().push(new SetFMChannelCommand(_app->window(), _app->project().getChannel(_index), *settings));

        delete settings;
    } else if (fileInfo.suffix() == "pcm") {
        PCMChannelSettings* settings = new PCMChannelSettings;
        settings->setPath(path);
        _app->undoStack().push(new SetPCMChannelCommand(_app->window(), _app->project().getChannel(_index), *settings));

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

                Track& t = _app->project().getFrontPattern().getTrack(_index);

                _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), t, items));
                t.usePianoRoll();
            }
        }
    }
}

const QColor& ChannelWidget::fmColor() const
{
    return _fmColor;
}

const QColor& ChannelWidget::toneColor() const
{
    return _toneColor;
}

const QColor& ChannelWidget::noiseColor() const
{
    return _noiseColor;
}

const QColor& ChannelWidget::pcmColor() const
{
    return _pcmColor;
}

void ChannelWidget::setFMColor(const QColor& color)
{
    _fmColor = color;

    if (_app->project().getChannel(_index).type() == Channel::FM) {
        ui->pushButton->setStyleSheet(QString("background-color: %1; color: %2;").arg(_fmColor.name()).arg(_fmColor.lightness() <= 96 ? "white" : "default"));
    }
}

void ChannelWidget::setToneColor(const QColor& color)
{
    _toneColor = color;

    if (_app->project().getChannel(_index).type() == Channel::TONE) {
        ui->pushButton->setStyleSheet(QString("background-color: %1; color: %2;").arg(_toneColor.name()).arg(_toneColor.lightness() <= 96 ? "white" : "default"));
    }
}

void ChannelWidget::setNoiseColor(const QColor& color)
{
    _noiseColor = color;

    if (_app->project().getChannel(_index).type() == Channel::NOISE) {
        ui->pushButton->setStyleSheet(QString("background-color: %1; color: %2;").arg(_noiseColor.name()).arg(_noiseColor.lightness() <= 96 ? "white" : "default"));
    }
}

void ChannelWidget::setPCMColor(const QColor& color)
{
    _pcmColor = color;

    if (_app->project().getChannel(_index).type() == Channel::PCM) {
        ui->pushButton->setStyleSheet(QString("background-color: %1; color: %2;").arg(_pcmColor.name()).arg(_pcmColor.lightness() <= 96 ? "white" : "default"));
    }
}

void ChannelWidget::ledClicked(bool shift)
{
    Channel& channel = _app->project().getChannel(_index);
    channel.setEnabled(!channel.enabled());

    ui->led->setOn(channel.enabled());

    if (shift) {
        emit ledShiftClicked();
    }
}

void ChannelWidget::buttonPressed()
{
    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        bool ok;
        const QString name = QInputDialog::getText(this, tr("Change Channel Name"), tr("Channel name:"), QLineEdit::Normal, _app->project().getChannel(_index).name(), &ok);
        if (ok && !name.isEmpty()) {
            _app->undoStack().push(new SetChannelNameCommand(_app->window(), _app->project().getChannel(_index), name));
            emit nameChanged();
        }
    } else {
        bool state = !ui->pushButton->isChecked();
        emit toggled(state);
    }
}

void ChannelWidget::buttonContextMenuRequested(const QPoint& p)
{
    const Pattern& frontPattern = _app->project().getFrontPattern();
    _pianoRollAction.setChecked(frontPattern.hasTrack(_index) && frontPattern.getTrack(_index).doesUsePianoRoll());

    _toneAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::TONE);
    _noiseAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::NOISE);
    _fmAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::FM);
    _pcmAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::PCM);

    _contextMenu.exec(mapToGlobal(p));
}

void ChannelWidget::pianoRollWasToggled()
{
    bool on = _pianoRollAction.isChecked();

    emit pianoRollTriggered(on);
}

void ChannelWidget::pianoRollWasTriggered()
{
    emit toggled(true);
    emit pianoRollTriggered(true);
}

void ChannelWidget::toneWasTriggered()
{
    _app->undoStack().push(new SetChannelTypeCommand(_app->window(), _app->project().getChannel(_index), Channel::Type::TONE));

    emit selected();
}

void ChannelWidget::noiseWasTriggered()
{
    _app->undoStack().push(new SetChannelTypeCommand(_app->window(), _app->project().getChannel(_index), Channel::Type::NOISE));

    emit selected();
}

void ChannelWidget::fmWasTriggered()
{
    _app->undoStack().push(new SetChannelTypeCommand(_app->window(), _app->project().getChannel(_index), Channel::Type::FM));

    emit selected();
}

void ChannelWidget::pcmWasTriggered()
{
    _app->undoStack().push(new SetChannelTypeCommand(_app->window(), _app->project().getChannel(_index), Channel::Type::PCM));

    emit selected();
}

void ChannelWidget::volumeDialChanged(const int val)
{
    _app->undoStack().push(new EditChannelVolumeCommand(_app->window(), _index, val));
}

void ChannelWidget::deleteTriggered()
{
    _app->undoStack().push(new DeleteChannelCommand(_app->window(), _app->project().getChannel(_index), _index));
}

void ChannelWidget::moveUpTriggered()
{
    if (_index != 0) {
        _app->undoStack().push(new MoveChannelUpCommand(_app->window(), _index));
    }
}

void ChannelWidget::moveDownTriggered()
{
    if (_index != _app->project().channelCount()-1) {
        _app->undoStack().push(new MoveChannelDownCommand(_app->window(), _index));
    }
}

void ChannelWidget::fillEvery2StepsTriggered()
{
    Track& track = _app->project().getFrontPattern().getTrack(_index);

    QList<Track::Item*> newItems;

    for (int i = 0; i < 8; i++) {
        float time = i * 0.5;
        newItems.append(new Track::Item(time, Note(60, 0.25)));
    }

    _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), track, newItems));
}

void ChannelWidget::fillEvery4StepsTriggered()
{
    Track& track = _app->project().getFrontPattern().getTrack(_index);

    QList<Track::Item*> newItems;

    for (int i = 0; i < 4; i++) {
        newItems.append(new Track::Item(i, Note(60, 0.25)));
    }

    _app->undoStack().push(new ReplaceTrackItemsCommand(_app->window(), track, newItems));
}

void ChannelWidget::paintEvent(QPaintEvent* event)
{
    if (_index < _app->project().channelCount()) {
        ui->pushButton->setText(_app->project().getChannel(_index).name());

        const Pattern& activePattern = _app->project().getFrontPattern();
        ui->trackStackedWidget->setCurrentIndex(activePattern.hasTrack(_index) && activePattern.getTrack(_index).doesUsePianoRoll());

        switch (_app->project().getChannel(_index).type()) {
            case Channel::TONE:
                ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_toneColor.name()));
                break;
            case Channel::NOISE:
                ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_noiseColor.name()));
                break;
            case Channel::FM:
                ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_fmColor.name()));
                break;
            case Channel::PCM:
                ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_pcmColor.name()));
                break;
        }
    }
    QWidget::paintEvent(event);
}

void ChannelWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void ChannelWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.replace("\r\n", "");

    fromPath(path);

    event->acceptProposedAction();
}
