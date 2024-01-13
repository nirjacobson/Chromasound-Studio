#include "editfm2settingscommand.h"
#include "mainwindow.h"

EditFM2SettingsCommand::EditFM2SettingsCommand(MainWindow* window, FM2Settings& settings, const FM2Settings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
    , _type(NONE)
{

}

void EditFM2SettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->doUpdate();
}

void EditFM2SettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->doUpdate();
}

int EditFM2SettingsCommand::id() const
{
    return 'Q';
}

bool EditFM2SettingsCommand::mergeWith(const QUndoCommand* other)
{
    const EditFM2SettingsCommand* esc;
    if ((esc = dynamic_cast<const EditFM2SettingsCommand*>(other))) {
        FM2Settings settings = _settingsAfter;
        settings.setTl(esc->_settingsAfter.tl());
        if (esc->_settingsAfter == settings) {
            if (_type == NONE || _type == TL) {
                _type = TL;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.setFb(esc->_settingsAfter.fb());
        if (esc->_settingsAfter == settings) {
            if (_type == NONE || _type == FB) {
                _type = FB;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}
