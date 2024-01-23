#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
    ui->bassSpinBox->setValue(settings.value(Chromasound_Studio::EqualizerBass, 0).toInt());
    ui->trebleSpinBox->setValue(settings.value(Chromasound_Studio::EqualizerTreble, 0).toInt());
    
    if (settings.value("format", Chromasound_Studio::Chromasound).toString() == Chromasound_Studio::Chromasound) {
        ui->chromasoundRadioButton->setChecked(true);
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
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
    
    settings.setValue(Chromasound_Studio::EqualizerBass, ui->bassSpinBox->value());
    settings.setValue(Chromasound_Studio::EqualizerTreble, ui->trebleSpinBox->value());
    settings.setValue(Chromasound_Studio::Format, ui->chromasoundRadioButton->isChecked() ? Chromasound_Studio::Chromasound : Chromasound_Studio::Standard);

    close();
}
