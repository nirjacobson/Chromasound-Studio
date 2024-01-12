#include "playlistchangewidget.h"
#include "ui_playlistchangewidget.h"

PlaylistChangeWidget::PlaylistChangeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaylistChangeWidget)
{
    ui->setupUi(this);

    connect(ui->doneButton, &QPushButton::pressed, this, &PlaylistChangeWidget::doneButtonClicked);
    connect(ui->removeButton, &QPushButton::pressed, this, &PlaylistChangeWidget::removeButtonClicked);

    connect(ui->lfoWidget, &LFOWidget::changed, this, &PlaylistChangeWidget::lfoChanged);
    connect(ui->ssgNoiseFreqWidget, &SSGNoiseFreqWidget::changed, this, &PlaylistChangeWidget::noiseFrequencyChanged);
    connect(ui->ssgEnvFreqWidget, &SSGEnvelopeFreqWidget::changed, this, &PlaylistChangeWidget::envelopeFrequencyChanged);
    connect(ui->ssgEnvShapeWidget, &SSGEnvelopeShapeWidget::changed, this, &PlaylistChangeWidget::envelopeShapeChanged);
}

PlaylistChangeWidget::~PlaylistChangeWidget()
{
    delete ui;
}

void PlaylistChangeWidget::setLFOChange(Playlist::LFOChange* change)
{
    _lfoChange = change;
    ui->lfoWidget->set(change->mode());
    ui->stackedWidget->setCurrentIndex(0);
}

void PlaylistChangeWidget::setNoiseFrequencyChange(Playlist::NoiseFrequencyChange* change)
{
    _noiseFrequencyChange = change;
    ui->ssgNoiseFreqWidget->set(change->frequency());
    ui->stackedWidget->setCurrentIndex(1);
}

void PlaylistChangeWidget::setEnvelopeFrequencyChange(Playlist::EnvelopeFrequencyChange* change)
{
    _envelopeFrequencyChange = change;
    ui->ssgEnvFreqWidget->set(change->frequency());
    ui->stackedWidget->setCurrentIndex(2);
}

void PlaylistChangeWidget::setEnvelopeShapeChange(Playlist::EnvelopeShapeChange* change)
{
    _envelopeShapeChange = change;
    ui->ssgEnvShapeWidget->set(change->shape());
    ui->stackedWidget->setCurrentIndex(3);
}

void PlaylistChangeWidget::lfoChanged()
{
    _lfoChange->setMode(ui->lfoWidget->setting());
}

void PlaylistChangeWidget::noiseFrequencyChanged()
{
    _noiseFrequencyChange->setFrequency(ui->ssgNoiseFreqWidget->setting());
}

void PlaylistChangeWidget::envelopeFrequencyChanged()
{
    _envelopeFrequencyChange->setFrequency(ui->ssgEnvFreqWidget->setting());
}

void PlaylistChangeWidget::envelopeShapeChanged()
{
    _envelopeShapeChange->setShape(ui->ssgEnvShapeWidget->settings());
}
