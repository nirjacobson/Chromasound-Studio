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
    , _toneColor(Qt::cyan)
    , _noiseColor(Qt::lightGray)
    , _fmColor(Qt::magenta)
{
    ui->setupUi(this);

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

    connect(&_toneAction, &QAction::triggered, this, &ChannelWidget::toneWasTriggered);
    connect(&_noiseAction, &QAction::triggered, this, &ChannelWidget::noiseWasTriggered);
    connect(&_fmAction, &QAction::triggered, this, &ChannelWidget::fmWasTriggered);

    connect(ui->stepKeys, &StepKeysWidget::clicked, this, &ChannelWidget::pianoKeyClicked);
    connect(ui->stepVelocities, &StepVelocitiesWidget::clicked, this, &ChannelWidget::velocityClicked);

    const Pattern& frontPattern = _app->project().getFrontPattern();
    _pianoRollAction.setCheckable(true);
    _pianoRollAction.setChecked(frontPattern.hasTrack(index) && frontPattern.getTrack(index).doesUsePianoRoll());
    _contextMenu.addAction(&_pianoRollAction);

    _contextMenu.addSeparator();
    _contextMenu.addAction(&_moveUpAction);
    _contextMenu.addAction(&_moveDownAction);

    _contextMenu.addSection("Type");
    _toneAction.setCheckable(true);
    _toneAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::TONE);
    _noiseAction.setCheckable(true);
    _noiseAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::NOISE);
    _fmAction.setCheckable(true);
    _fmAction.setChecked(_app->project().getChannel(index).type() == Channel::Type::FM);
    _typeActionGroup.addAction(&_toneAction);
    _typeActionGroup.addAction(&_noiseAction);
    _typeActionGroup.addAction(&_fmAction);
    _contextMenu.addAction(&_toneAction);
    _contextMenu.addAction(&_noiseAction);
    _contextMenu.addAction(&_fmAction);

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
    }

   ui->pushButton->setText(_app->project().getChannel(_index).name());
   ui->stepSequencer->setIndex(_index);
   ui->prDisplay->setIndex(_index);
   ui->led->setOn(_app->project().getChannel(_index).enabled());
   ui->volumeDial->setValue(_app->project().getChannel(_index).settings().volume());
   ui->rectLed->setOnFunction([=](){
       float appPosition = _app->position();

       QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(appPosition);
       const Pattern& frontPattern = _app->project().getFrontPattern();
       int frontPatternIdx = _app->project().frontPattern();
       if (frontPattern.hasTrack(_index)) {
           const Track& track = frontPattern.getTrack(_index);
           if (_app->project().playMode() == Project::PlayMode::PATTERN) {
               return _app->project().getChannel(_index).enabled() &&
                       frontPattern.activeTracksAtTime(appPosition).contains(_index) &&
                       std::find_if(track.items().begin(),
                                 track.items().end(),
                        [&](const Track::Item* item){
                           float delta = item->time() - appPosition;
                           return qAbs(delta) <= 0.0625; }) == track.items().end();
           } else {
              return activePatterns.contains(_app->project().frontPattern()) &&
                     _app->project().getChannel(_index).enabled() &&
                     frontPattern.activeTracksAtTime(appPosition - activePatterns[frontPatternIdx]).contains(_index) &&
                     std::find_if(track.items().begin(), track.items().end(),
                     [&](const Track::Item* item){
                       float delta = item->time() - (appPosition - activePatterns[frontPatternIdx]);
                       return qAbs(delta) <= 0.0625; }) == track.items().end();
           }
       }

       return false;
   });

   ui->stepKeys->setChannel(_index);
   ui->stepVelocities->setChannel(_index);

   _noiseAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::NOISE);
   _toneAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::TONE);
   _fmAction.setChecked(_app->project().getChannel(_index).type() == Channel::Type::FM);

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

void ChannelWidget::setFMColor(const QColor& color)
{
    _fmColor = color;

    if (_app->project().getChannel(_index).type() == Channel::FM) {
        ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_fmColor.name()));
    }
}

void ChannelWidget::setToneColor(const QColor& color)
{
    _toneColor = color;

    if (_app->project().getChannel(_index).type() == Channel::TONE) {
        ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_toneColor.name()));
    }
}

void ChannelWidget::setNoiseColor(const QColor& color)
{
    _noiseColor = color;


    if (_app->project().getChannel(_index).type() == Channel::NOISE) {
        ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_noiseColor.name()));
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
            _app->project().getChannel(_index).setName(name);
            ui->pushButton->setText(name);
        }
        emit nameChanged();
    } else {
        bool state = !ui->pushButton->isChecked();
        emit toggled(state);
    }
}

void ChannelWidget::buttonContextMenuRequested(const QPoint& p)
{
    _contextMenu.exec(mapToGlobal(p));
}

void ChannelWidget::pianoRollWasToggled()
{
    bool on = _pianoRollAction.isChecked();

    if (on) {
        emit toggled(true);
    }

    emit pianoRollTriggered(on);
}

void ChannelWidget::pianoRollWasTriggered()
{
    emit toggled(true);
    emit pianoRollTriggered(true);
}

void ChannelWidget::toneWasTriggered()
{
    ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_toneColor.name()));
    emit toneTriggered();
}

void ChannelWidget::noiseWasTriggered()
{
    ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_noiseColor.name()));
    emit noiseTriggered();
}

void ChannelWidget::fmWasTriggered()
{
    ui->pushButton->setStyleSheet(QString("background-color: %1;").arg(_fmColor.name()));
    emit fmTriggered();
}

void ChannelWidget::volumeDialChanged(const int val)
{
    _app->undoStack().push(new EditChannelVolumeCommand(_app->window(), _index, val));
}

void ChannelWidget::paintEvent(QPaintEvent* event)
{
    const Pattern& activePattern = _app->project().getFrontPattern();
    ui->trackStackedWidget->setCurrentIndex(activePattern.hasTrack(_index) && activePattern.getTrack(_index).doesUsePianoRoll());

    QWidget::paintEvent(event);
}
