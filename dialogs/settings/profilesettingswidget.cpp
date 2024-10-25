#include "profilesettingswidget.h"
#include "ui_profilesettingswidget.h"

ProfileSettingsWidget::ProfileSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfileSettingsWidget)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
    QList<QString> presets = {
        Chromasound_Studio::ChromasoundNova,
        Chromasound_Studio::ChromasoundNovaDirect,
        Chromasound_Studio::ChromasoundPro,
        Chromasound_Studio::ChromasoundProDirect,
        Chromasound_Studio::Genesis,
        Chromasound_Studio::MSX2,
        Chromasound_Studio::Custom
    };

    QString preset = settings.value(Chromasound_Studio::ProfilePresetKey, Chromasound_Studio::Custom).toString();
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString());
    Chromasound_Studio::Profile profile(isChromasound, pcmStrategy, discretePCM, usePCMSRAM);

    ui->presetComboBox->setCurrentIndex(presets.indexOf(preset));
    ui->chromasoundCheckBox->setChecked(profile.isChromasound());
    ui->pcmStrategyComboBox->setCurrentIndex(profile.pcmStrategy());
    ui->discretePCMCheckBox->setChecked(profile.discretePCM());
    ui->pcmSRAMCheckBox->setChecked(profile.usePCMSRAM());

    connect(ui->presetComboBox, &QComboBox::currentIndexChanged, this, &ProfileSettingsWidget::presetChanged);
    connect(ui->chromasoundCheckBox, &QCheckBox::clicked, this, &ProfileSettingsWidget::checkboxClicked);
    connect(ui->discretePCMCheckBox, &QCheckBox::clicked, this, &ProfileSettingsWidget::checkboxClicked);
    connect(ui->pcmSRAMCheckBox, &QCheckBox::clicked, this, &ProfileSettingsWidget::checkboxClicked);
    connect(ui->pcmStrategyComboBox, &QComboBox::currentIndexChanged, this, &ProfileSettingsWidget::pcmStrategyChanged);
}

ProfileSettingsWidget::~ProfileSettingsWidget()
{
    delete ui;
}

QString ProfileSettingsWidget::preset() const
{
    switch (ui->presetComboBox->currentIndex()) {
    case 0:
        return Chromasound_Studio::ChromasoundNova;
    case 1:
        return Chromasound_Studio::ChromasoundNovaDirect;
    case 2:
        return Chromasound_Studio::ChromasoundPro;
    case 3:
        return Chromasound_Studio::ChromasoundProDirect;
    case 4:
        return Chromasound_Studio::Genesis;
    case 5:
        return Chromasound_Studio::MSX2;
    case 6:
        return Chromasound_Studio::Custom;
    }

    return Chromasound_Studio::Custom;
}

Chromasound_Studio::Profile ProfileSettingsWidget::profile() const
{
    return Chromasound_Studio::Profile(
        ui->chromasoundCheckBox->isChecked(),
        static_cast<Chromasound_Studio::PCMStrategy>(ui->pcmStrategyComboBox->currentIndex()),
        ui->discretePCMCheckBox->isChecked(),
        ui->pcmSRAMCheckBox->isChecked());
}

void ProfileSettingsWidget::checkboxClicked(bool)
{
    ui->presetComboBox->blockSignals(true);
    ui->presetComboBox->setCurrentIndex(ui->presetComboBox->count() - 1);
    ui->presetComboBox->blockSignals(false);
}

void ProfileSettingsWidget::pcmStrategyChanged(const int index)
{
    ui->presetComboBox->blockSignals(true);
    ui->presetComboBox->setCurrentIndex(ui->presetComboBox->count() - 1);
    ui->presetComboBox->blockSignals(false);
}

void ProfileSettingsWidget::presetChanged(const int index)
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    QString preset = settings.value(Chromasound_Studio::ProfilePresetKey, Chromasound_Studio::Custom).toString();
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString());
    Chromasound_Studio::Profile defaultProfile(isChromasound, pcmStrategy, discretePCM, usePCMSRAM);

    const Chromasound_Studio::Profile* profile;
    switch (index) {
    case 0:
        profile = &Chromasound_Studio::ChromasoundNovaPreset;
        break;
    case 1:
        profile = &Chromasound_Studio::ChromasoundNovaDirectPreset;
        break;
    case 2:
        profile = &Chromasound_Studio::ChromasoundProPreset;
        break;
    case 3:
        profile = &Chromasound_Studio::ChromasoundProDirectPreset;
        break;
    case 4:
        profile = &Chromasound_Studio::GenesisPreset;
        break;
    case 5:
        profile = &Chromasound_Studio::MSX2Preset;
        break;
    default:
        profile = &defaultProfile;
        break;
    }

    ui->chromasoundCheckBox->blockSignals(true);
    ui->chromasoundCheckBox->setChecked(profile->isChromasound());
    ui->chromasoundCheckBox->blockSignals(false);

    ui->pcmStrategyComboBox->blockSignals(true);
    ui->pcmStrategyComboBox->setCurrentIndex(profile->pcmStrategy());
    ui->pcmStrategyComboBox->blockSignals(false);

    ui->discretePCMCheckBox->blockSignals(true);
    ui->discretePCMCheckBox->setChecked(profile->discretePCM());
    ui->discretePCMCheckBox->blockSignals(false);

    ui->pcmSRAMCheckBox->blockSignals(true);
    ui->pcmSRAMCheckBox->setChecked(profile->usePCMSRAM());
    ui->pcmSRAMCheckBox->blockSignals(false);
}
