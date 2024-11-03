#ifndef EMULATOROUTPUTDEVICESETTINGSWIDGET_H
#define EMULATOROUTPUTDEVICESETTINGSWIDGET_H

#include <QWidget>
#include <QSettings>

#include "chromasound/emu/audio_output.h"
#include "Chromasound_Studio.h"

namespace Ui {
class EmulatorOutputDeviceSettingsWidget;
}

class EmulatorOutputDeviceSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmulatorOutputDeviceSettingsWidget(QWidget *parent = nullptr);
    ~EmulatorOutputDeviceSettingsWidget();

    QString outputDevice() const;

    void doUpdate();

private:
    Ui::EmulatorOutputDeviceSettingsWidget *ui;
};

#endif // EMULATOROUTPUTDEVICESETTINGSWIDGET_H
