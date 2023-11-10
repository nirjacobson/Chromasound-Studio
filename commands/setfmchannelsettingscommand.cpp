#include "setfmchannelsettingscommand.h"
#include "mainwindow.h"

SetFMChannelSettingsCommand::SetFMChannelSettingsCommand(MainWindow* window, FMChannelSettings& settings, const FMChannelSettings& settingsAfter)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(settingsAfter)
{
    setText("set channel settings");
}

void SetFMChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void SetFMChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();

}
