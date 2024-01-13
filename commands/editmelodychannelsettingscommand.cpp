#include "editmelodychannelsettingscommand.h"
#include "mainwindow.h"

EditMelodyChannelSettingsCommand::EditMelodyChannelSettingsCommand(MainWindow* window, MelodyChannelSettings& settings, const MelodyChannelSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{

}

void EditMelodyChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditMelodyChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
