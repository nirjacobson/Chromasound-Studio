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

    QString outputDevice = settings.value(Chromasound_Studio::OutputDeviceKey, QString::fromStdString(AudioOutput<int16_t>::instance()->devices()[AudioOutput<int16_t>::instance()->defaultDeviceIndex()])).toString();
    ui->outputDeviceComboBox->setCurrentText(outputDevice);
}

EmulatorOutputDeviceSettingsWidget::~EmulatorOutputDeviceSettingsWidget()
{
    delete ui;
}

QString EmulatorOutputDeviceSettingsWidget::outputDevice() const
{
    return ui->outputDeviceComboBox->currentText();
}

void EmulatorOutputDeviceSettingsWidget::doUpdate()
{
    ui->outputDeviceComboBox->blockSignals(true);
    ui->outputDeviceComboBox->clear();

    bool mustInit = !AudioOutput<int16_t>::instance()->isInited();

    if (mustInit) {
        AudioOutput<int16_t>::instance()->init();
    }

    AudioOutput<int16_t>::instance()->init();

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
    QString outputDevice = settings.value(Chromasound_Studio::OutputDeviceKey, QString::fromStdString(AudioOutput<int16_t>::instance()->devices()[AudioOutput<int16_t>::instance()->defaultDeviceIndex()])).toString();
    ui->outputDeviceComboBox->setCurrentText(outputDevice);

    ui->outputDeviceComboBox->blockSignals(false);

    if (mustInit) {
        AudioOutput<int16_t>::instance()->destroy();
    }
}
