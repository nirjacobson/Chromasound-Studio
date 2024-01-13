#include "editfm2operatorsettingscommand.h"
#include "mainwindow.h"

EditFM2OperatorSettingsCommand::EditFM2OperatorSettingsCommand(MainWindow* window, FM2OperatorSettings& settings, const FM2OperatorSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
    , _type(NONE)
{

}

void EditFM2OperatorSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->doUpdate();
}

void EditFM2OperatorSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->doUpdate();
}

int EditFM2OperatorSettingsCommand::id() const
{
    return 'P';
}

bool EditFM2OperatorSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const EditFM2OperatorSettingsCommand* eosc;
    if ((eosc = dynamic_cast<const EditFM2OperatorSettingsCommand*>(other))) {
        FM2OperatorSettings settings = _settingsAfter;
        settings.envelopeSettings().setAr(eosc->_settingsAfter.envelopeSettings().ar());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == AR) {
                _type = AR;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.envelopeSettings().setDr(eosc->_settingsAfter.envelopeSettings().dr());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == DR) {
                _type = DR;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.envelopeSettings().setSl(eosc->_settingsAfter.envelopeSettings().sl());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == SL) {
                _type = SL;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.envelopeSettings().setRr(eosc->_settingsAfter.envelopeSettings().rr());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == RR) {
                _type = RR;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.setMulti(eosc->_settingsAfter.multi());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == MULTI) {
                _type = MULTI;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.setKsl(eosc->_settingsAfter.ksl());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == KSL) {
                _type = KSL;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}
