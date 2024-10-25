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
    ui->audioBufferSizeSpinBox->setValue(settings.value(Chromasound_Studio::AudioBufferSizeKey, 256).toInt());
    ui->readBufferSizeSpinBox->setValue(settings.value(Chromasound_Studio::ReadBufferSizeKey, 1).toInt());
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

int EmulationSettingsWidget::readBufferSize() const
{
    return ui->readBufferSizeSpinBox->value();
}
