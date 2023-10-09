#include "channelwidget.h"
#include "ui_channelwidget.h"
#include <QtDebug>

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
{
    ui->setupUi(this);

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

    connect(ui->rectLed, &RectLED::clicked, this, &ChannelWidget::rectLedClicked);
    connect(ui->rectLed, &RectLED::doubleClicked, this, &ChannelWidget::rectLedDoubleClicked);

    connect(&_pianoRollAction, &QAction::triggered, this, &ChannelWidget::pianoRollTriggered);
    connect(&_moveUpAction, &QAction::triggered, this, &ChannelWidget::moveUpTriggered);
    connect(&_moveDownAction, &QAction::triggered, this, &ChannelWidget::moveDownTriggered);
    connect(&_deleteAction, &QAction::triggered, this, &ChannelWidget::deleteTriggered);

    connect(&_toneAction, &QAction::triggered, this, &ChannelWidget::toneTriggered);
    connect(&_noiseAction, &QAction::triggered, this, &ChannelWidget::noiseTriggered);
    connect(&_fmAction, &QAction::triggered, this, &ChannelWidget::fmTriggered);

    connect(ui->stepKeys, &StepKeysWidget::clicked, this, &ChannelWidget::pianoKeyClicked);
    connect(ui->stepVelocities, &StepVelocitiesWidget::clicked, this, &ChannelWidget::velocityClicked);

    _contextMenu.addAction(&_pianoRollAction);

    _contextMenu.addSeparator();
    _contextMenu.addAction(&_moveUpAction);
    _contextMenu.addAction(&_moveDownAction);

    _contextMenu.addSection("Type");
    _toneAction.setCheckable(true);
    _noiseAction.setCheckable(true);
    _fmAction.setCheckable(true);
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

   ui->pushButton->setText(_app->project().getChannel(_index).name());
   ui->stepSequencer->setIndex(_index);
   ui->prDisplay->setIndex(_index);
   ui->led->setOn(_app->project().getChannel(_index).enabled());
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

   update();
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
        bool state = ui->pushButton->isChecked();
        emit toggled(!state);
    }
}

void ChannelWidget::buttonContextMenuRequested(const QPoint& p)
{
    _contextMenu.exec(mapToGlobal(p));
}

void ChannelWidget::paintEvent(QPaintEvent* event)
{
    const Pattern& activePattern = _app->project().getFrontPattern();
    ui->trackStackedWidget->setCurrentIndex(activePattern.hasTrack(_index) && activePattern.getTrack(_index).doesUsePianoRoll());

    QWidget::paintEvent(event);
}
