#include "editssgchannelsettingscommand.h"
#include "mainwindow.h"

EditSSGChannelSettingsCommand::EditSSGChannelSettingsCommand(MainWindow* window, SSGChannelSettings& settings, const SSGChannelSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{

}

void EditSSGChannelSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditSSGChannelSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}
