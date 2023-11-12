#include "mainwindow.h"
#include "setpcmchannelsettingscommand.h"

SetPCMChannelSettingsCommand::SetPCMChannelSettingsCommand(MainWindow* window, PCMChannelSettings& settings, const PCMChannelSettings& settingsAfter)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(settingsAfter)
{
    setText("set channel settings");
}

void SetPCMChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void SetPCMChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
