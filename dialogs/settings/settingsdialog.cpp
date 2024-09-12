#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accepted);
    connect(ui->chromasoundLayoutWidget, &ChromasoundLayoutWidget::changed, this, &SettingsDialog::chromasoundLayoutChanged);

    chromasoundLayoutChanged();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accepted()
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    settings.setValue(Chromasound_Studio::EqualizerBass, ui->emulationSettingsWidget->bass());
    settings.setValue(Chromasound_Studio::EqualizerTreble, ui->emulationSettingsWidget->treble());
    settings.setValue(Chromasound_Studio::AudioBufferSize, ui->emulationSettingsWidget->audioBufferSize());
    settings.setValue(Chromasound_Studio::ReadBufferSize, ui->emulationSettingsWidget->readBufferSize());
    settings.setValue(Chromasound_Studio::Format, ui->playbackSettingsWidget->format());
    settings.setValue(Chromasound_Studio::NumberOfChromasounds, ui->chromasoundLayoutWidget->quantity());
    settings.setValue(Chromasound_Studio::Chromasound1, ui->chromasoundLayoutWidget->chromasound1());
    settings.setValue(Chromasound_Studio::Chromasound2, ui->chromasoundLayoutWidget->chromasound2());
    settings.setValue(Chromasound_Studio::OutputDevice, ui->emulatorOutputDeviceWidget->outputDeviceIndex());

    emit done();

    close();
}

void SettingsDialog::chromasoundLayoutChanged()
{
    bool showEmuSettings = (ui->chromasoundLayoutWidget->chromasound1() == Chromasound_Studio::Emulator || (ui->chromasoundLayoutWidget->chromasound2() == Chromasound_Studio::Emulator && ui->chromasoundLayoutWidget->quantity() == 2));
    ui->outputDeviceGroupBox->setVisible(showEmuSettings);
    ui->emuAdjustmentsGroupBox->setVisible(showEmuSettings);
}
