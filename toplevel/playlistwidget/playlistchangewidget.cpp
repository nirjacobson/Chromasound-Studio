#include "playlistchangewidget.h"
#include "ui_playlistchangewidget.h"

PlaylistChangeWidget::PlaylistChangeWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::PlaylistChangeWidget)
    , _app(app)
    , _lfoChange(nullptr)
    , _noiseFrequencyChange(nullptr)
    , _envelopeFrequencyChange(nullptr)
    , _envelopeShapeChange(nullptr)
    , _userToneChange(nullptr)
{
    ui->setupUi(this);

    ui->userToneWidget->setApplication(app);

    connect(ui->doneButton, &QPushButton::pressed, this, &PlaylistChangeWidget::doneButtonClicked);
    connect(ui->removeButton, &QPushButton::pressed, this, &PlaylistChangeWidget::removeButtonClicked);

    connect(ui->lfoWidget, &LFOWidget::changed, this, &PlaylistChangeWidget::lfoChanged);
    connect(ui->ssgNoiseFreqWidget, &SSGNoiseFreqWidget::changed, this, &PlaylistChangeWidget::noiseFrequencyChanged);
    connect(ui->ssgEnvFreqWidget, &SSGEnvelopeFreqWidget::changed, this, &PlaylistChangeWidget::envelopeFrequencyChanged);
    connect(ui->ssgEnvShapeWidget, &SSGEnvelopeShapeWidget::changed, this, &PlaylistChangeWidget::envelopeShapeChanged);
    connect(ui->userToneWidget, &OPLWidget::changed, this, &PlaylistChangeWidget::userToneChanged);
}

PlaylistChangeWidget::~PlaylistChangeWidget()
{
    delete ui;
}

void PlaylistChangeWidget::setApplication(Application* app)
{
    _app = app;

    ui->userToneWidget->setApplication(app);
}

void PlaylistChangeWidget::setLFOChange(Playlist::LFOChange* change)
{
    _lfoChange = change;
    _noiseFrequencyChange = nullptr;
    _envelopeFrequencyChange = nullptr;
    _envelopeShapeChange = nullptr;
    _userToneChange = nullptr;

    ui->lfoWidget->set(change->mode());
    ui->stackedWidget->setCurrentIndex(0);
}

void PlaylistChangeWidget::setNoiseFrequencyChange(Playlist::NoiseFrequencyChange* change)
{
    _lfoChange = nullptr;
    _noiseFrequencyChange = change;
    _envelopeFrequencyChange = nullptr;
    _envelopeShapeChange = nullptr;
    _userToneChange = nullptr;

    ui->ssgNoiseFreqWidget->set(change->frequency());
    ui->stackedWidget->setCurrentIndex(1);
}

void PlaylistChangeWidget::setEnvelopeFrequencyChange(Playlist::EnvelopeFrequencyChange* change)
{
    _lfoChange = nullptr;
    _noiseFrequencyChange = nullptr;
    _envelopeFrequencyChange = change;
    _envelopeShapeChange = nullptr;
    _userToneChange = nullptr;

    _envelopeFrequencyChange = change;
    ui->ssgEnvFreqWidget->set(change->frequency());
    ui->stackedWidget->setCurrentIndex(2);
}

void PlaylistChangeWidget::setEnvelopeShapeChange(Playlist::EnvelopeShapeChange* change)
{
    _lfoChange = nullptr;
    _noiseFrequencyChange = nullptr;
    _envelopeFrequencyChange = nullptr;
    _envelopeShapeChange = change;
    _userToneChange = nullptr;

    ui->ssgEnvShapeWidget->set(change->shape());
    ui->stackedWidget->setCurrentIndex(3);
}

void PlaylistChangeWidget::setUserToneChange(Playlist::UserToneChange* change)
{
    _lfoChange = nullptr;
    _noiseFrequencyChange = nullptr;
    _envelopeFrequencyChange = nullptr;
    _envelopeShapeChange = nullptr;
    _userToneChange = change;

    ui->userToneWidget->setSettings(&change->userTone());
    ui->stackedWidget->setCurrentIndex(4);
}

void PlaylistChangeWidget::doUpdate()
{
    if (_lfoChange) {
        ui->lfoWidget->set(_lfoChange->mode());
    }

    if (_noiseFrequencyChange) {
        ui->ssgNoiseFreqWidget->set(_noiseFrequencyChange->frequency());
    }

    if (_envelopeFrequencyChange) {
        ui->ssgEnvFreqWidget->set(_envelopeFrequencyChange->frequency());
    }

    if (_envelopeShapeChange) {
        ui->ssgEnvShapeWidget->set(_envelopeShapeChange->shape());
    }

    if (_userToneChange) {
        ui->userToneWidget->setSettings(&_userToneChange->userTone());
    }
}

void PlaylistChangeWidget::lfoChanged()
{
    _app->undoStack().push(new EditPlaylistLFOChangeCommand(_app->window(), _lfoChange, ui->lfoWidget->setting()));
}

void PlaylistChangeWidget::noiseFrequencyChanged()
{
    _app->undoStack().push(new EditPlaylistNoiseFrequencyChangeCommand(_app->window(), _noiseFrequencyChange, ui->ssgNoiseFreqWidget->setting()));
}

void PlaylistChangeWidget::envelopeFrequencyChanged()
{
    _app->undoStack().push(new EditPlaylistEnvelopeFrequencyChangeCommand(_app->window(), _envelopeFrequencyChange, ui->ssgEnvFreqWidget->setting()));
}

void PlaylistChangeWidget::envelopeShapeChanged()
{
    _app->undoStack().push(new EditSSGEnvelopeSettingsCommand(_app->window(), _envelopeShapeChange->shape(), ui->ssgEnvShapeWidget->settings()));
}

void PlaylistChangeWidget::userToneChanged()
{
    _app->undoStack().push(new EditOPLSettingsCommand(_app->window(), _userToneChange->userTone(), ui->userToneWidget->settings()));
}
