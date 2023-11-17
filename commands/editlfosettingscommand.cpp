#include "editlfosettingscommand.h"
#include "mainwindow.h"

EditLFOSettingsCommand::EditLFOSettingsCommand(MainWindow* window, LFOSettings& settings, const LFOSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{
    setText("edit channel settings");
}

void EditLFOSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditLFOSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}

int EditLFOSettingsCommand::id() const
{
    return 'L';
}

bool EditLFOSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const EditLFOSettingsCommand* elsc;
    if ((elsc = dynamic_cast<const EditLFOSettingsCommand*>(other))) {
        LFOSettings settings = _settingsAfter;
        settings.setAMS(elsc->_settingsAfter.ams());
        if (settings == elsc->_settingsAfter) {
            _settingsAfter = elsc->_settingsAfter;
            return true;
        }

        settings = _settingsAfter;
        settings.setFMS(elsc->_settingsAfter.fms());
        if (settings == elsc->_settingsAfter) {
            _settingsAfter = elsc->_settingsAfter;
            return true;
        }
    }

    return false;
}
