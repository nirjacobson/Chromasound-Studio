#include "editnoisechannelsettingscommand.h"
#include "mainwindow.h"

EditNoiseChannelSettingsCommand::EditNoiseChannelSettingsCommand(MainWindow* window, NoiseChannelSettings& settings, const NoiseChannelSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{
    setText("edit channel settings");
}

void EditNoiseChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditNoiseChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
