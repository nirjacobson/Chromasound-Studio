#include "editrhythmchannelsettingscommand.h"
#include "mainwindow.h"

EditRhythmChannelSettingsCommand::EditRhythmChannelSettingsCommand(MainWindow* window, RhythmChannelSettings& settings, const RhythmChannelSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{

}

void EditRhythmChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditRhythmChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
