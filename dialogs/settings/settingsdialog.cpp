#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accepted);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accepted()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.setValue(Chromasound_Studio::EqualizerBass, ui->emulationSettingsWidget->bass());
    settings.setValue(Chromasound_Studio::EqualizerTreble, ui->emulationSettingsWidget->treble());
    settings.setValue(Chromasound_Studio::AudioBufferSize, ui->emulationSettingsWidget->audioBufferSize());
    settings.setValue(Chromasound_Studio::ReadBufferSize, ui->emulationSettingsWidget->readBufferSize());
    settings.setValue(Chromasound_Studio::Format, ui->playbackSettingsWidget->format());
    settings.setValue(Chromasound_Studio::NumberOfChromasounds, ui->chromasoundLayoutWidget->quantity());
    settings.setValue(Chromasound_Studio::Chromasound1, ui->chromasoundLayoutWidget->chromasound1());
    settings.setValue(Chromasound_Studio::Chromasound2, ui->chromasoundLayoutWidget->chromasound2());

    emit done();

    close();
}
