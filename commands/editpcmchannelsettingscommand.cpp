#include "editpcmchannelsettingscommand.h"
#include "mainwindow.h"

EditPCMChannelSettingsCommand::EditPCMChannelSettingsCommand(MainWindow* window, PCMChannelSettings& settings, const PCMChannelSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{
    setText("edit channel settings");
}

void EditPCMChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditPCMChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
