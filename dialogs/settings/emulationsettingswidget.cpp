#include "emulationsettingswidget.h"
#include "ui_emulationsettingswidget.h"

EmulationSettingsWidget::EmulationSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmulationSettingsWidget)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
    ui->bassSpinBox->setValue(settings.value(Chromasound_Studio::EqualizerBassKey, 0).toInt());
    ui->trebleSpinBox->setValue(settings.value(Chromasound_Studio::EqualizerTrebleKey, 0).toInt());
    ui->audioBufferSizeSpinBox->setValue(settings.value(Chromasound_Studio::AudioBufferSizeKey, 1024).toInt());
    ui->playbackReadBufferSizeSpinBox->setValue(settings.value(Chromasound_Studio::PlaybackReadBufferSizeKey, 1).toInt());
    ui->interactiveReadBufferSizeSpinBox->setValue(settings.value(Chromasound_Studio::InteractiveReadBufferSizeKey, 1).toInt());
}

EmulationSettingsWidget::~EmulationSettingsWidget()
{
    delete ui;
}

int EmulationSettingsWidget::bass() const
{
    return ui->bassSpinBox->value();
}

int EmulationSettingsWidget::treble() const
{
    return ui->trebleSpinBox->value();
}

int EmulationSettingsWidget::audioBufferSize() const
{
    return ui->audioBufferSizeSpinBox->value();
}

int EmulationSettingsWidget::playbackReadBufferSize() const
{
    return ui->playbackReadBufferSizeSpinBox->value();
}

int EmulationSettingsWidget::interactiveReadBufferSize() const
{
    return ui->interactiveReadBufferSizeSpinBox->value();
}

void EmulationSettingsWidget::setDual(const bool dual)
{
    ui->playbackReadBufferSizeLabel->setText(dual ? "Playback read buffer size" : "Read buffer size");
    ui->interactiveReadBufferSizeLabel->setVisible(dual);
    ui->interactiveReadBufferSizeSpinBox->setVisible(dual);
    ui->interactiveAudioBuffersLabel->setVisible(dual);
}
