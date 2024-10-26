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
    QList<QString> devices = {
        Chromasound_Studio::ChromasoundNova,
        Chromasound_Studio::ChromasoundNovaDirect,
        Chromasound_Studio::ChromasoundPro,
        Chromasound_Studio::ChromasoundProDirect,
        Chromasound_Studio::Genesis,
        Chromasound_Studio::MSX2
    };

    QString device = settings.value(Chromasound_Studio::DeviceKey, Chromasound_Studio::ChromasoundNova).toString();
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString().toLower());
    Chromasound_Studio::Profile profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    deviceChanged(devices.indexOf(device));

    connect(ui->deviceComboBox, &QComboBox::currentIndexChanged, this, &ProfileSettingsWidget::deviceChanged);
    connect(ui->pcmStrategyComboBox, &QComboBox::currentTextChanged, this, &ProfileSettingsWidget::pcmStrategyChanged);

    QStringList strategies = {
        "None",
        "Inline",
        "Sequential",
        "Random"
    };

    ui->deviceComboBox->setCurrentIndex(devices.indexOf(device));
    ui->chromasoundCheckBox->setChecked(profile.isChromasound());
    ui->pcmStrategyComboBox->setCurrentText(strategies[profile.pcmStrategy()]);
    ui->discretePCMCheckBox->setChecked(profile.discretePCM());
    ui->pcmSRAMCheckBox->setChecked(profile.usePCMSRAM());
}

ProfileSettingsWidget::~ProfileSettingsWidget()
{
    delete ui;
}

QString ProfileSettingsWidget::device() const
{
    switch (ui->deviceComboBox->currentIndex()) {
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
    }

    return Chromasound_Studio::ChromasoundNova;
}

Chromasound_Studio::Profile ProfileSettingsWidget::profile() const
{
    return Chromasound_Studio::Profile(
        Chromasound_Studio::pcmStrategyFromString(ui->pcmStrategyComboBox->currentText().toLower()),
        ui->chromasoundCheckBox->isChecked(),
        ui->discretePCMCheckBox->isChecked(),
        ui->pcmSRAMCheckBox->isChecked());
}

void ProfileSettingsWidget::deviceChanged(const int index)
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    QString device = settings.value(Chromasound_Studio::DeviceKey, Chromasound_Studio::ChromasoundNova).toString();
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString());
    Chromasound_Studio::Profile currentProfile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    QStringList strategies = {
        "None",
        "Inline",
        "Sequential",
        "Random"
    };

    const Chromasound_Studio::Profile* profile;
    switch (index) {
    case 0:
        profile = &Chromasound_Studio::ChromasoundNovaPreset;
        strategies.removeOne("None");
        strategies.removeOne("Random");
        ui->chromasoundCheckBox->setVisible(true);
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 1:
        profile = &Chromasound_Studio::ChromasoundNovaDirectPreset;
        strategies.removeOne("None");
        strategies.removeOne("Random");
        strategies.removeOne("Sequential");
        ui->chromasoundCheckBox->setVisible(true);
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 2:
        profile = &Chromasound_Studio::ChromasoundProPreset;
        strategies.removeOne("None");
        ui->chromasoundCheckBox->setVisible(true);
        ui->discretePCMCheckBox->setVisible(true);
        ui->pcmSRAMCheckBox->setVisible(true);
        break;
    case 3:
        profile = &Chromasound_Studio::ChromasoundProDirectPreset;
        strategies.removeOne("None");
        ui->chromasoundCheckBox->setVisible(true);
        ui->discretePCMCheckBox->setVisible(true);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 4:
        profile = &Chromasound_Studio::GenesisPreset;
        strategies = { "Sequential" };
        ui->chromasoundCheckBox->setVisible(false);
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 5:
        profile = &Chromasound_Studio::MSX2Preset;
        strategies = { "None" };
        ui->chromasoundCheckBox->setVisible(false);
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    default:
        profile = &currentProfile;
        break;
    }

    ui->pcmStrategyComboBox->clear();
    ui->pcmStrategyComboBox->addItems(strategies);

    QStringList strategiesLower = strategies;

    for (QString& str : strategiesLower) {
        str = str.toLower();
    }

    ui->pcmStrategyComboBox->setCurrentIndex(strategiesLower.indexOf(Chromasound_Studio::pcmStrategyToString(profile->pcmStrategy())));

    ui->chromasoundCheckBox->blockSignals(true);
    ui->chromasoundCheckBox->setChecked(profile->isChromasound());
    ui->chromasoundCheckBox->blockSignals(false);

    ui->discretePCMCheckBox->blockSignals(true);
    ui->discretePCMCheckBox->setChecked(profile->discretePCM());
    ui->discretePCMCheckBox->blockSignals(false);

    ui->pcmSRAMCheckBox->blockSignals(true);
    ui->pcmSRAMCheckBox->setChecked(profile->usePCMSRAM());
    ui->pcmSRAMCheckBox->blockSignals(false);
}

void ProfileSettingsWidget::pcmStrategyChanged(const QString& value)
{
    if (ui->deviceComboBox->currentIndex() == 2 || ui->deviceComboBox->currentIndex() == 3) {
        if (value == "Inline") {
            ui->pcmSRAMCheckBox->setChecked(false);
        } else if (value == "Random") {
            ui->pcmSRAMCheckBox->setChecked(true);
        }
        ui->pcmSRAMCheckBox->setVisible(value == "Sequential" && ui->deviceComboBox->currentIndex() != 3);
    }
}
