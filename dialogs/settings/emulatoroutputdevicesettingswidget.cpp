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

    ui->outputDeviceComboBox->setCurrentText(settings.value(Chromasound_Studio::OutputDevice, AudioOutput<int16_t>::instance()->defaultDevice().c_str()).toString());

}

EmulatorOutputDeviceSettingsWidget::~EmulatorOutputDeviceSettingsWidget()
{
    delete ui;
}


QString EmulatorOutputDeviceSettingsWidget::outputDevice() const
{
    return ui->outputDeviceComboBox->currentText();
}
