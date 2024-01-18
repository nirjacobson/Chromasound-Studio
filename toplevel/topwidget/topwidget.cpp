#include "topwidget.h"
#include "ui_topwidget.h"

TopWidget::TopWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::TopWidget)
    , _midiInput(MIDIInput::instance())
    , _isPlaying(false)
{
    ui->setupUi(this);

    if (app != nullptr) {
        setApplication(app);
    }

    for (const QString& deviceName : _midiInput->devices()) {
        ui->deviceComboBox->addItem(deviceName);
    }

    ui->deviceComboBox->setCurrentIndex(_midiInput->device());

    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaStop));

    connect(ui->deviceComboBox, &QComboBox::currentIndexChanged, this, &TopWidget::midiDeviceSet);
    connect(ui->patRadioButton, &QRadioButton::clicked, this, &TopWidget::patModeSelected);
    connect(ui->songRadioButton, &QRadioButton::clicked, this, &TopWidget::songModeSelected);
    connect(ui->playButton, &QPushButton::clicked, this, &TopWidget::playPauseClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &TopWidget::stopClicked);
    connect(ui->patSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(patternChanged(int)));
    connect(ui->tempoSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(tempoChanged(int)));
    connect(ui->tempoSpinBox, SIGNAL(valueChanged(int)), this, SLOT(tempoDidChange(int)));
    connect(ui->beatsPerBarSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(beatsPerBarChanged(int)));
    connect(ui->beatsPerBarSpinBox, SIGNAL(valueChanged(int)), this, SLOT(beatsPerBarDidChange(int)));
}

TopWidget::~TopWidget()
{
    delete ui;
}

void TopWidget::setApplication(Application* app)
{
    _app = app;
    ui->seekWidget->setApplication(app);
    ui->positionDisplay->setApplication(app);
    ui->tempoSpinBox->setValue(app->project().tempo());
    ui->beatsPerBarSpinBox->setValue(app->project().beatsPerBar());
    connect(&app->fmPSG(), &FM_PSG::stopped, this, &TopWidget::fmpsgStopped);
}

void TopWidget::updateFromProject(const Project& project)
{
    ui->patSpinBox->blockSignals(true);
    ui->patSpinBox->setValue(project.frontPattern()+1);
    ui->patSpinBox->blockSignals(false);

    ui->tempoSpinBox->blockSignals(true);
    ui->tempoSpinBox->setValue(project.tempo());
    ui->tempoSpinBox->blockSignals(false);

    ui->beatsPerBarSpinBox->blockSignals(true);
    ui->beatsPerBarSpinBox->setValue(project.beatsPerBar());
    ui->beatsPerBarSpinBox->blockSignals(false);

    if (project.playMode() == Project::PlayMode::PATTERN) {
        ui->patRadioButton->blockSignals(true);
        ui->patRadioButton->setChecked(true);
        ui->patRadioButton->blockSignals(false);
    } else {
        ui->songRadioButton->blockSignals(true);
        ui->songRadioButton->setChecked(true);
        ui->songRadioButton->blockSignals(false);
    }
}

void TopWidget::setStatusMessage(const QString& message)
{
    ui->statusLabel->setText(message);
}

void TopWidget::doUpdate(const float position)
{
    ui->seekWidget->doUpdate(position);
    ui->positionDisplay->doUpdate(position);
}

void TopWidget::playPauseClicked()
{
    _isPlaying = !_isPlaying;

    ui->playButton->setIcon(ui->playButton->style()->standardIcon(_isPlaying ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));

    if (_isPlaying) {
        emit play();
    } else {
        emit pause();
    }
}

void TopWidget::stopClicked()
{
    _isPlaying = false;
    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));

    emit stop();
}

void TopWidget::tempoDidChange(int bpm)
{
    _app->project().setTempo(bpm);
}

void TopWidget::beatsPerBarDidChange(int beats)
{
    _app->project().setBeatsPerBar(beats);
}

void TopWidget::patModeSelected()
{
    _app->project().setPlayMode(Project::PlayMode::PATTERN);
}

void TopWidget::songModeSelected()
{
    _app->project().setPlayMode(Project::PlayMode::SONG);
}

void TopWidget::fmpsgStopped()
{
    _isPlaying = false;
    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));
}

void TopWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

