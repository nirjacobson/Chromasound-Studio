#include "resetfmchannelsettingscommand.h"
#include "mainwindow.h"

ResetFMChannelSettingsCommand::ResetFMChannelSettingsCommand(MainWindow* window, FMChannelSettings& settings)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
{
    setText("reset channel settings");
}

void ResetFMChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void ResetFMChannelSettingsCommand::redo()
{
    _settings = FMChannelSettings();

    _mainWindow->channelSettingsUpdated();

}
