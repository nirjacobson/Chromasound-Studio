#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QSettings settings(FM_PSG_Studio::Organization, FM_PSG_Studio::Application);
    ui->bassSpinBox->setValue(settings.value(FM_PSG_Studio::EqualizerBass, 0).toInt());
    ui->trebleSpinBox->setValue(settings.value(FM_PSG_Studio::EqualizerTreble, 0).toInt());

    if (settings.value("format", FM_PSG_Studio::FM_PSG).toString() == FM_PSG_Studio::FM_PSG) {
        ui->fmPSGRadioButton->setChecked(true);
    } else {
        ui->standardRadioButton->setChecked(true);
    }

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accepted);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accepted()
{
    QSettings settings(FM_PSG_Studio::Organization, FM_PSG_Studio::Application);

    settings.setValue(FM_PSG_Studio::EqualizerBass, ui->bassSpinBox->value());
    settings.setValue(FM_PSG_Studio::EqualizerTreble, ui->trebleSpinBox->value());
    settings.setValue(FM_PSG_Studio::Format, ui->fmPSGRadioButton->isChecked() ? FM_PSG_Studio::FM_PSG : FM_PSG_Studio::Standard);

    close();
}
