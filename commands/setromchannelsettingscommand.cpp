#include "mainwindow.h"
#include "setromchannelsettingscommand.h"

SetROMChannelSettingsCommand::SetROMChannelSettingsCommand(MainWindow* window, ROMChannelSettings& settings, const ROMChannelSettings& settingsAfter)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(settingsAfter)
{
    setText("set channel settings");
}

void SetROMChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void SetROMChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
