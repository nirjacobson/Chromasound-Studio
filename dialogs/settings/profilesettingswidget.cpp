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

    QString device = settings.value(Chromasound_Studio::DeviceKey, Chromasound_Studio::ChromasoundPro).toString();
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString().toLower());
    Chromasound_Studio::Profile profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    QStringList strategies = {
        "None",
        "Inline",
        "Sequential",
        "Random"
    };

    connect(ui->pcmStrategyComboBox, &QComboBox::currentIndexChanged, this, &ProfileSettingsWidget::comboBoxChanged);
    connect(ui->deviceComboBox, &QComboBox::currentIndexChanged, this, &ProfileSettingsWidget::deviceComboBoxChanged);
    connect(ui->deviceComboBox, &QComboBox::currentIndexChanged, this, &ProfileSettingsWidget::comboBoxChanged);

    comboBoxChanged(devices.indexOf(device));
    deviceComboBoxChanged(devices.indexOf(device));

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

void ProfileSettingsWidget::comboBoxChanged(const int index)
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString());
    Chromasound_Studio::Profile currentProfile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    const Chromasound_Studio::Profile* profile;
    switch (ui->deviceComboBox->currentIndex()) {
    case 0:
        profile = &Chromasound_Studio::ChromasoundNovaPreset;
        ui->chromasoundCheckBox->setVisible(ui->pcmStrategyComboBox->currentText() != "Inline");
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 1:
        profile = &Chromasound_Studio::ChromasoundNovaDirectPreset;
        ui->chromasoundCheckBox->setVisible(ui->pcmStrategyComboBox->currentText() != "Inline");
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 2:
        profile = &Chromasound_Studio::ChromasoundProPreset;
        ui->chromasoundCheckBox->setVisible(ui->pcmStrategyComboBox->currentText() != "Inline");
        ui->discretePCMCheckBox->setVisible(true);
        ui->pcmSRAMCheckBox->setVisible(ui->pcmStrategyComboBox->currentText() == "Sequential");

        if (ui->pcmStrategyComboBox->currentText() == "Inline") {
            ui->pcmSRAMCheckBox->setChecked(false);
        }

        break;
    case 3:
        profile = &Chromasound_Studio::ChromasoundProDirectPreset;
        ui->chromasoundCheckBox->setVisible(ui->pcmStrategyComboBox->currentText() != "Inline");
        ui->discretePCMCheckBox->setVisible(true);
        ui->pcmSRAMCheckBox->setVisible(false);

        if (ui->pcmStrategyComboBox->currentText() == "Inline") {
            ui->pcmSRAMCheckBox->setChecked(false);
        }

        break;
    case 4:
        profile = &Chromasound_Studio::GenesisPreset;
        ui->chromasoundCheckBox->setVisible(false);
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    case 5:
        profile = &Chromasound_Studio::MSX2Preset;
        ui->chromasoundCheckBox->setVisible(false);
        ui->discretePCMCheckBox->setVisible(false);
        ui->pcmSRAMCheckBox->setVisible(false);
        break;
    default:
        profile = &currentProfile;
        break;
    }
}

void ProfileSettingsWidget::deviceComboBoxChanged(const int index)
{
    QStringList strategies = {
        "None",
        "Inline",
        "Sequential",
        "Random"
    };

    const Chromasound_Studio::Profile* profile;
    switch (ui->deviceComboBox->currentIndex()) {
    case 0:
        profile = &Chromasound_Studio::ChromasoundNovaPreset;
        strategies.removeOne("None");
        strategies.removeOne("Random");
        break;
    case 1:
        profile = &Chromasound_Studio::ChromasoundNovaDirectPreset;
        strategies.removeOne("None");
        strategies.removeOne("Random");
        strategies.removeOne("Sequential");
        break;
    case 2:
        profile = &Chromasound_Studio::ChromasoundProPreset;
        strategies.removeOne("None");
        break;
    case 3:
        profile = &Chromasound_Studio::ChromasoundProDirectPreset;
        strategies.removeOne("None");
        break;
    case 4:
        profile = &Chromasound_Studio::GenesisPreset;
        strategies = { "Sequential" };
        break;
    case 5:
        profile = &Chromasound_Studio::MSX2Preset;
        strategies = { "None" };
        break;
    default:
        break;
    }

    ui->chromasoundCheckBox->setChecked(profile->isChromasound());
    ui->discretePCMCheckBox->setChecked(profile->discretePCM());
    ui->pcmSRAMCheckBox->setChecked(profile->usePCMSRAM());

    QStringList strategiesLower = strategies;

    for (QString& str : strategiesLower) {
        str = str.toLower();
    }

    ui->pcmStrategyComboBox->blockSignals(true);
    ui->pcmStrategyComboBox->clear();
    ui->pcmStrategyComboBox->addItems(strategies);
    ui->pcmStrategyComboBox->blockSignals(false);

    ui->pcmStrategyComboBox->setCurrentIndex(strategiesLower.indexOf(Chromasound_Studio::pcmStrategyToString(profile->pcmStrategy())));
}
