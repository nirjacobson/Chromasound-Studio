#include "editalgorithmsettingscommand.h"
#include "mainwindow.h"

EditAlgorithmSettingsCommand::EditAlgorithmSettingsCommand(MainWindow* window, AlgorithmSettings& settings, const AlgorithmSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{
    setText("edit channel settings");
}

void EditAlgorithmSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditAlgorithmSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}

int EditAlgorithmSettingsCommand::id() const
{
    return 'A';
}

bool EditAlgorithmSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const EditAlgorithmSettingsCommand* easc;
    if ((easc = dynamic_cast<const EditAlgorithmSettingsCommand*>(other))) {
        AlgorithmSettings settings = _settingsBefore;
        settings.setAlgorithm(_settingsAfter.algorithm());
        if (_settingsAfter == settings) {
            return false;
        }

        settings = _settingsAfter;
        settings.setFeedback(easc->_settingsAfter.feedback());
        if (easc->_settingsAfter == settings) {
            _settingsAfter = easc->_settingsAfter;
            return true;
        }
    }

    return false;
}
