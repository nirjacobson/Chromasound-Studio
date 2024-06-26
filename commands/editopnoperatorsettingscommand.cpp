#include "editopnoperatorsettingscommand.h"
#include "mainwindow.h"

EditOPNOperatorSettingsCommand::EditOPNOperatorSettingsCommand(MainWindow* window, OPNOperatorSettings& settings, const OPNOperatorSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
    , _type(NONE)
{
    setText("edit channel settings");
}

void EditOPNOperatorSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->channelSettingsUpdated();
}

void EditOPNOperatorSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->channelSettingsUpdated();
}

int EditOPNOperatorSettingsCommand::id() const
{
    return 'O';
}

bool EditOPNOperatorSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const EditOPNOperatorSettingsCommand* eosc;
    if ((eosc = dynamic_cast<const EditOPNOperatorSettingsCommand*>(other))) {
        OPNOperatorSettings settings = _settingsAfter;
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
        settings.envelopeSettings().setT1l(eosc->_settingsAfter.envelopeSettings().t1l());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == T1L) {
                _type = T1L;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.envelopeSettings().setD1r(eosc->_settingsAfter.envelopeSettings().d1r());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == D1R) {
                _type = D1R;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.envelopeSettings().setT2l(eosc->_settingsAfter.envelopeSettings().t2l());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == T2L) {
                _type = T2L;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.envelopeSettings().setD2r(eosc->_settingsAfter.envelopeSettings().d2r());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == D2R) {
                _type = D2R;
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
        settings.setRs(eosc->_settingsAfter.rs());
        if (eosc->_settingsAfter == settings) {
            if (_type == NONE || _type == RS) {
                _type = RS;
                _settingsAfter = eosc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}
