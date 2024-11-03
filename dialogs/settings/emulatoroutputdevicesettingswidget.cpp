#include "emulatoroutputdevicesettingswidget.h"
#include "ui_emulatoroutputdevicesettingswidget.h"

EmulatorOutputDeviceSettingsWidget::EmulatorOutputDeviceSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmulatorOutputDeviceSettingsWidget)
{
    ui->setupUi(this);

    std::vector<std::string> devices = AudioOutput<int16_t>::instance()->devices();
    QStringList devicesList;
    for (std::string& dev : devices) {
        devicesList.append(dev.c_str());
    }
    ui->outputDeviceComboBox->addItems(devicesList);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    ui->outputDeviceComboBox->setCurrentIndex(settings.value(Chromasound_Studio::OutputDeviceKey, AudioOutput<int16_t>::instance()->defaultDeviceIndex()).toInt());

}

EmulatorOutputDeviceSettingsWidget::~EmulatorOutputDeviceSettingsWidget()
{
    delete ui;
}

int EmulatorOutputDeviceSettingsWidget::outputDeviceIndex() const
{
    return ui->outputDeviceComboBox->currentIndex();
}


void EmulatorOutputDeviceSettingsWidget::doUpdate()
{
    ui->outputDeviceComboBox->blockSignals(true);
    ui->outputDeviceComboBox->clear();

    AudioOutput<int16_t>::instance()->init();

    std::vector<std::string> devices = AudioOutput<int16_t>::instance()->devices();
    QStringList devicesList;
    for (std::string& dev : devices) {
        devicesList.append(dev.c_str());
    }
    ui->outputDeviceComboBox->addItems(devicesList);

    AudioOutput<int16_t>::instance()->destroy();

    ui->outputDeviceComboBox->blockSignals(false);
}

